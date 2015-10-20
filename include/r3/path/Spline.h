#ifndef R3_PATH_SPLINE_H_
#define R3_PATH_SPLINE_H_

#include <cstddef>
#include <vector>
#include <memory>
#include <Eigen/Core>
#include <Eigen/QR>
#include <Eigen/Sparse>
#include <Eigen/StdVector>

namespace r3 {
namespace path {

template <
  class _Scalar = double,
  class _Index = ptrdiff_t,
  _Index _NumCoefficients = Eigen::Dynamic,
  _Index _NumOutputs = Eigen::Dynamic,
  _Index _NumKnots = Eigen::Dynamic>
class SplineND
{
public:
  using Scalar = _Scalar;
  using Index = _Index;
  
  static constexpr Index NumCoefficientsAtCompileTime = _NumCoefficients;
  static constexpr Index NumOutputsAtCompileTime = _NumOutputs;
  static constexpr Index NumKnotsAtCompileTime= _NumKnots;
  static constexpr Index NumSegmentsAtCompileTime
    = (_NumKnots != Eigen::Dynamic)
      ? (NumKnotsAtCompileTime - 1)
      : Eigen::Dynamic;
  static constexpr Index DimensionAtCompileTime
    = (NumSegmentsAtCompileTime != Eigen::Dynamic && _NumCoefficients != Eigen::Dynamic)
      ? (NumSegmentsAtCompileTime * NumCoefficientsAtCompileTime)
      : Eigen::Dynamic;

  using TimeVector = Eigen::Matrix<Scalar, NumKnotsAtCompileTime, 1>;
  using SolutionMatrix = Eigen::Matrix<Scalar, NumOutputsAtCompileTime, NumCoefficientsAtCompileTime>;
  using OutputVector = Eigen::Matrix<Scalar, NumOutputsAtCompileTime, 1>;

  SplineND(
    const TimeVector& _times,
    const std::vector<SolutionMatrix,
      Eigen::aligned_allocator<SolutionMatrix> > &_solution);

  // Default copy and move semantics.
  SplineND(SplineND&& _other) = default;
  SplineND(const SplineND& _other) = default;
  SplineND& operator =(SplineND&& _other) = default;
  SplineND& operator =(const SplineND& _other) = default;

  Index getNumKnots() const;
  Index getNumOutputs() const;
  Index getNumDerivatives() const;
  Index getNumCoefficients() const;

  Scalar getDuration() const;

  Index getSegmentIndex(Scalar _t) const;
  OutputVector evaluate(Scalar _t, Index _derivative = 0) const;

private:
  using CoefficientVector = Eigen::Matrix<Scalar, NumCoefficientsAtCompileTime, 1>;
  using CoefficientMatrix = Eigen::Matrix<Scalar, NumCoefficientsAtCompileTime, NumCoefficientsAtCompileTime>;

  TimeVector mTimes;
  std::vector<SolutionMatrix,
    Eigen::aligned_allocator<SolutionMatrix> > mSolution; // length _NumSegments

public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW_IF(TimeVector::NeedsToAlign);
};

template <
  class _Scalar = double,
  class _Index = ptrdiff_t,
  _Index _NumCoefficients = Eigen::Dynamic,
  _Index _NumOutputs = Eigen::Dynamic,
  _Index _NumKnots = Eigen::Dynamic>
class SplineProblem
{
public:
  using Scalar = _Scalar;
  using Index = _Index;

  static constexpr Index NumCoefficientsAtCompileTime = _NumCoefficients;
  static constexpr Index NumOutputsAtCompileTime = _NumOutputs;
  static constexpr Index NumKnotsAtCompileTime= _NumKnots;
  static constexpr Index NumSegmentsAtCompileTime
    = (_NumKnots != Eigen::Dynamic)
      ? (NumKnotsAtCompileTime - 1)
      : Eigen::Dynamic;
  static constexpr Index DimensionAtCompileTime
    = (NumSegmentsAtCompileTime != Eigen::Dynamic && _NumCoefficients != Eigen::Dynamic)
      ? (NumSegmentsAtCompileTime * NumCoefficientsAtCompileTime)
      : Eigen::Dynamic;

  using TimeVector = Eigen::Matrix<Scalar, NumKnotsAtCompileTime, 1>;
  using OutputVector = Eigen::Matrix<Scalar, NumOutputsAtCompileTime, 1>;
  using OutputMatrix = Eigen::Matrix<Scalar, NumCoefficientsAtCompileTime, NumOutputsAtCompileTime>;
  using CoefficientVector = Eigen::Matrix<Scalar, NumCoefficientsAtCompileTime, 1>;
  using CoefficientMatrix = Eigen::Matrix<Scalar, NumCoefficientsAtCompileTime, NumCoefficientsAtCompileTime>;
  using ProblemMatrix = Eigen::SparseMatrix<Scalar, 0, Index>;
  using ProblemVector = Eigen::Matrix<Scalar, DimensionAtCompileTime, NumOutputsAtCompileTime>;
  using SolutionMatrix = Eigen::Matrix<Scalar, NumOutputsAtCompileTime, NumCoefficientsAtCompileTime>;
  using Spline = SplineND<Scalar, Index, _NumCoefficients, _NumOutputs, _NumKnots>;

  explicit SplineProblem(const TimeVector& _times);
  SplineProblem(const TimeVector& _times, Index _numCoefficients, Index _numOutputs);

  // Default copy and move semantics.
  SplineProblem(SplineProblem&& _other) = default;
  SplineProblem(const SplineProblem& _other) = default;
  SplineProblem& operator =(SplineProblem&& _other) = default;
  SplineProblem& operator =(const SplineProblem& _other) = default;

  static CoefficientVector createTimeVector(Scalar _t, Index _i, Index _n);
  static CoefficientMatrix createCoefficientMatrix(Index _n);

  void addConstantConstraint(Index _knot, Index _derivative, const OutputVector& _value);
  void addContinuityConstraint(Index _knot, Index _derivative);

  Spline fit();

  // These belong on a "Spline" class.
  Index getNumKnots() const;
  Index getNumOutputs() const;
  Scalar getDuration() const;

private:
  Index mNumKnots;
  Index mNumSegments;
  Index mNumCoefficients;
  Index mNumOutputs;
  Index mDimension;

  CoefficientMatrix mCoefficientMatrix;

  Index mRowIndex;
  TimeVector mTimes;
  ProblemMatrix mA;
  ProblemVector mB;

  std::vector<SolutionMatrix,
    Eigen::aligned_allocator<SolutionMatrix> > mSolution; // length _NumSegments

public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW_IF(
       CoefficientMatrix::NeedsToAlign
    || TimeVector::NeedsToAlign
    || ProblemMatrix::NeedsToAlign
    || ProblemVector::NeedsToAlign
  );
};

// ---

class Trajectory
{
public:
  using Scalar = double;
  using Index = ptrdiff_t;

  virtual ~Trajectory() = default;

  virtual Index getNumOutputs() const = 0;

  virtual Index getNumDerivatives() const = 0;

  virtual Scalar getDuration() const = 0;

  virtual Eigen::VectorXd evaluate(Scalar _t, Index _derivative) const = 0;
};

using TrajectoryPtr = std::shared_ptr<Trajectory>;
using ConstTrajectoryPtr = std::shared_ptr<const Trajectory>;

// ---

template <class _Spline>
class SplineTrajectory : public virtual Trajectory {
public:
  using Spline = _Spline;

  explicit SplineTrajectory(const Spline& _spline);
  explicit SplineTrajectory(Spline&& _spline);
  virtual ~SplineTrajectory() = default;

  // Default copy and move semantics.
  SplineTrajectory(SplineTrajectory&& _other) = default;
  SplineTrajectory(const SplineTrajectory& _other) = default;
  SplineTrajectory& operator =(SplineTrajectory&& _other) = default;
  SplineTrajectory& operator =(const SplineTrajectory& _other) = default;

  Index getNumOutputs() const override;

  Index getNumDerivatives() const override;

  Scalar getDuration() const override;

  Eigen::VectorXd evaluate(Scalar _t, Index _derivative) const override;

private:
  Spline mSpline;
};

} // namespace path
} // namespace r3

#include "detail/Spline-impl.h"

#endif // ifndef R3_PATH_SPLINE_H_

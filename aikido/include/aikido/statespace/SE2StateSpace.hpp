#ifndef AIKIDO_STATESPACE_SE2STATESPACE_H
#define AIKIDO_STATESPACE_SE2STATESPACE_H
#include <Eigen/Geometry>
#include "StateSpace.hpp"
#include "ScopedState.hpp"

namespace aikido {
namespace statespace {

template <class> class SE2StateHandle;

class SE2StateSpace
  : public std::enable_shared_from_this<SE2StateSpace>
  , public virtual StateSpace  
{
public:
  class State : public StateSpace::State
  {
  public:
    using Isometry2d = Eigen::Transform<double, 2, 
                                        Eigen::Isometry, Eigen::DontAlign>;
    
    // Constructs identity element
    State();
    ~State() = default;

    /// Constructs a point in SE(2) from a transfomation.
    explicit State(const Isometry2d& _transform);

    /// Sets value to a transfomation.
    void setIsometry(const Isometry2d& _transform);

    /// Gets value to a transfomation.
    const Isometry2d& getIsometry() const;

  private:
    Isometry2d mTransform;

    friend class SE2StateSpace;

  };

  using StateHandle = SE2StateHandle<State>;
  using StateHandleConst = SE2StateHandle<const State>;

  using ScopedState = statespace::ScopedState<StateHandle>;
  using ScopedStateConst = statespace::ScopedState<StateHandleConst>;

  using Isometry2d = State::Isometry2d;
  using Bounds = Eigen::Matrix2d;

  SE2StateSpace();

  SE2StateSpace(const Bounds& _translationBounds);

  ScopedState createState() const;

  /// Gets translational bounds on the StateSpace.
  Bounds getTranslationalBounds() const;

  /// Gets value as a transformation.
  const Isometry2d& getIsometry(const State* _state) const;

  /// Sets value to a transformation.
  void setIsometry(State* _state, const Isometry2d& _transform) const;

  // Documentation inherited.
  size_t getStateSizeInBytes() const override;

  // Documentation inherited.
  StateSpace::State* allocateStateInBuffer(void* _buffer) const override;

  // Documentation inherited.
  void freeStateInBuffer(StateSpace::State* _state) const override;

  // Documentation inherited.
  SampleableConstraintPtr createSampleableConstraint(
    std::unique_ptr<util::RNG> _rng) const override;
  
  // Documentation inherited.
  void compose(
    const StateSpace::State* _state1, const StateSpace::State* _state2,
    StateSpace::State* _out) const override;

  // Documentation inherited. _tangent should be 3d twist (w, v).
  void expMap(
    const Eigen::VectorXd& _tangent, StateSpace::State* _out) const override;

  /// Documentation inherited.
  int getDimension() const override;

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

private:
  Bounds mBounds;
};

} // namespace statespace
} // namespace aikido

#include "detail/SE2StateSpace.hpp"

#endif

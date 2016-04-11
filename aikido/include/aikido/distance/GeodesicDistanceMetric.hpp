#ifndef AIKIDO_GEODESIC_DISTANCE_H_
#define AIKIDO_GEODESIC_DISTANCE_H_

#include <aikido/distance/DistanceMetric.hpp>
#include <aikido/statespace/SO3StateSpace.hpp>

namespace aikido
{
namespace distance
{
/// Implements a distance metric on SO(3)
class GeodesicDistanceMetric : public DistanceMetric
{
public:
  /// Constructor.
  GeodesicDistanceMetric(std::shared_ptr<statespace::SO3StateSpace> _space);

  /// Computes distance between two states. This function satisfies
  /// the properties of a metric and its
  /// return value will always be between 0 and getMaximumExtent()
  virtual double distance(
      const aikido::statespace::StateSpace::State* _state1,
      const aikido::statespace::StateSpace::State* _state2) const override;

  /// Computes the state that lies at time t in [0, 1] on the segment
  /// that connects from state to to state. The memory location of state
  /// is not required to be different from the memory of either from or to.
  virtual void interpolate(
      const aikido::statespace::StateSpace::State* _from,
      const aikido::statespace::StateSpace::State* _to, const double _t,
      aikido::statespace::StateSpace::State* _state) const override;

private:
  std::shared_ptr<statespace::SO3StateSpace> mStateSpace;
};
};
}
#endif

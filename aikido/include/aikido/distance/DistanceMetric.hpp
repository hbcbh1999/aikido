#ifndef AIKIDO_DISTANCE_METRIC_H_
#define AIKIDO_DISTANCE_METRIC_H_

#include <aikido/statespace/StateSpace.hpp>

namespace aikido
{
namespace distance
{
/// Implements a distance metric defined on a StateSpace
class DistanceMetric
{
public:
  /// Computes distance between two states. This function satisfies
  /// the properties of a metric and its
  /// return value will always be between 0 and getMaximumExtent()
  virtual double distance(
      const aikido::statespace::StateSpace::State* _state1,
      const aikido::statespace::StateSpace::State* _state2) const = 0;

  /// Computes the state that lies at time t in [0, 1] on the segment
  /// that connects from state to to state. The memory location of state
  /// is not required to be different from the memory of either from or to.
  virtual void interpolate(
      const aikido::statespace::StateSpace::State* _from,
      const aikido::statespace::StateSpace::State* _to, const double _t,
      aikido::statespace::StateSpace::State* _state) const = 0;
};

using DistanceMetricPtr = std::shared_ptr<DistanceMetric>;
}
}
#endif

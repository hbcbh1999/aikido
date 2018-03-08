#ifndef AIKIDO_PLANNER_SNAPCONFIGURATIONTOCONFIGURATIONPLANNER_HPP_
#define AIKIDO_PLANNER_SNAPCONFIGURATIONTOCONFIGURATIONPLANNER_HPP_

#include "aikido/planner/ConfigurationToConfiguration.hpp"
#include "aikido/planner/ConfigurationToConfigurationPlanner.hpp"

namespace aikido {
namespace planner {

/// Planner that checks the straight-line trajectory to the goal.
///
/// SnapPlanner is a utility planner class that collision checks the
/// straight-line trajectory to the goal. If that trajectory is invalid, e.g.,
/// due to an environment or self collision, the planner immediately returns
/// \c nullptr.
///
/// SnapPlanner is intended to be used only as a "short circuit" to speed-up
/// planning between nearby configurations. This planner is most commonly used
/// as the first item in a Sequence meta-planner to avoid calling a motion
/// planner when the trivial solution is valid.
class SnapConfigurationToConfigurationPlanner
    : public ConfigurationToConfigurationPlanner
{
public:
  /// Constructor
  SnapConfigurationToConfigurationPlanner() = default;

  /// Solves ConfigurationToConfiguration problem.
  ///
  /// The planner returns success if the resulting trajectory satisfies
  /// constraint at some resolution and failure (returning \c nullptr)
  /// otherwise. The reason for the failure is stored in the \c result output
  /// parameter.
  ///
  /// \param problem Planning problem.
  /// \param[out] result Information about success or failure.
  /// \return Trajectory or \c nullptr if planning failed.
  /// \throw If \c problem is not ConfigurationToConfiguration.
  /// \throw If \c result is not ConfigurationToConfiguration::Result.
  trajectory::TrajectoryPtr plan(
      const Problem& problem, Result* result = nullptr) override;
};

} // namespace planner
} // namespace aikido

#endif // AIKIDO_PLANNER_SNAPCONFIGURATIONTOCONFIGURATIONPLANNER_HPP_
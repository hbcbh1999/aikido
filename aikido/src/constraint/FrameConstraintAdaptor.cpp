#include <aikido/constraint/FrameConstraintAdaptor.hpp>
#include <aikido/statespace/SE3StateSpace.hpp>

namespace aikido {
namespace constraint {


//=============================================================================
FrameConstraintAdaptor::FrameConstraintAdaptor(
  statespace::dart::MetaSkeletonStateSpacePtr _metaSkeletonStateSpace,
  dart::dynamics::ConstJacobianNodePtr _jacobianNode,
  DifferentiablePtr _poseConstraint)
: mJacobianNode(std::move(_jacobianNode))
, mPoseConstraint(std::move(_poseConstraint))
, mMetaSkeletonStateSpace(std::move(_metaSkeletonStateSpace))
{
  if (!mPoseConstraint)
    throw std::invalid_argument("_poseConstraint is nullptr.");

  if (!mJacobianNode)
    throw std::invalid_argument("_jacobianNode is nullptr.");

  if (!mMetaSkeletonStateSpace)
    throw std::invalid_argument("_metaSkeletonStateSpace is nullptr.");

  using SE3StateSpace = statespace::SE3StateSpace;

  auto space = dynamic_cast<SE3StateSpace*>(
    mPoseConstraint->getStateSpace().get());

  if (!space)
    throw std::invalid_argument("_poseConstraint is not in SE3StateSpace.");

  mMetaSkeleton = mMetaSkeletonStateSpace->getMetaSkeleton();

  if (!mMetaSkeleton)
    throw std::invalid_argument("_metaSkeletonStateSpace does not have skeleton.");

  // TODO: If possible, check that _frame is influenced by at least 
  // one DegreeOfFreedom in the _stateSpace's Skeleton.
}

//=============================================================================
size_t FrameConstraintAdaptor::getConstraintDimension() const
{
  return mPoseConstraint->getConstraintDimension();
}

//=============================================================================
Eigen::VectorXd FrameConstraintAdaptor::getValue(
  const statespace::StateSpace::State* _s) const
{
  using State = statespace::CompoundStateSpace::State;
  using SE3State = statespace::SE3StateSpace::State;

  auto state = static_cast<const State*>(_s);
  
  mMetaSkeletonStateSpace->setState(state);

  SE3State bodyPose(mJacobianNode->getTransform());

  return mPoseConstraint->getValue(&bodyPose);

}

//=============================================================================
Eigen::MatrixXd FrameConstraintAdaptor::getJacobian(
  const statespace::StateSpace::State* _s) const
{
  using State = statespace::CompoundStateSpace::State;
  using SE3State = statespace::SE3StateSpace::State;
  using dart::dynamics::MetaSkeletonPtr;

  auto state = static_cast<const State*>(_s);

  mMetaSkeletonStateSpace->setState(state);

  SE3State bodyPose(mJacobianNode->getTransform());
 
  // m x 6 matrix, Jacobian of constraints w.r.t. SE3 pose (se3 tangent vector)
  // where the tangent vector is expressed in world frame.
  Eigen::MatrixXd constraintJac = mPoseConstraint->getJacobian(&bodyPose);

  // 6 x numDofs, Jacobian of SE3 pose of body node expressed in World Frame.
  Eigen::MatrixXd skeletonJac = mMetaSkeleton->getWorldJacobian(mJacobianNode);

  // m x numDofs, Jacobian of pose w.r.t. generalized coordinates.
  return constraintJac*skeletonJac;

}

//=============================================================================
std::pair<Eigen::VectorXd, Eigen::MatrixXd> FrameConstraintAdaptor::getValueAndJacobian(
    const statespace::StateSpace::State* _s) const 
{
  using State = statespace::CompoundStateSpace::State;
  using SE3State = statespace::SE3StateSpace::State;
  using dart::dynamics::MetaSkeletonPtr;

  auto state = static_cast<const State*>(_s);

  mMetaSkeletonStateSpace->setState(state);

  SE3State bodyPose(mJacobianNode->getTransform());

  Eigen::VectorXd value = mPoseConstraint->getValue(&bodyPose);
 
  // m x 6 matrix, Jacobian of constraints w.r.t. SE3 pose (se3 tangent vector)
  // where the tangent vector is expressed in world frame.
  Eigen::MatrixXd constraintJac = mPoseConstraint->getJacobian(&bodyPose);

  // 6 x numDofs, Jacobian of SE3 pose of body node expressed in World Frame.
  Eigen::MatrixXd skeletonJac = mMetaSkeleton->getWorldJacobian(mJacobianNode);

  // m x numDofs, Jacobian of pose w.r.t. generalized coordinates.
  Eigen::MatrixXd jacobian = constraintJac*skeletonJac;

  return std::make_pair(value, jacobian);
}

//=============================================================================
std::vector<ConstraintType> FrameConstraintAdaptor::getConstraintTypes() const
{
  return mPoseConstraint->getConstraintTypes();
}


//=============================================================================
statespace::StateSpacePtr FrameConstraintAdaptor::getStateSpace() const
{
  return mMetaSkeletonStateSpace;
}

}
}

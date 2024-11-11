#include "Animation/UMSequenceStructs.h"

#include "Components/PoseableMeshComponent.h"



FUMJointKey UMSequenceHelper::MakeKeyframe(float Time, const FTransform &Transform)
{
	return {Time, Transform};
}

FUMJoint UMSequenceHelper::MakeJoint(FName NameIn, const FRotatorRange& RangeLimitsIn,
                                     const FUMJointTimeline& SequenceIn)
{ return {NameIn, RangeLimitsIn, SequenceIn}; }

const FUMJointGroup& UMSequenceHelper::MakeJointGroup(FName Name, const TArray<FUMJointGroup>& Groups,
                                                      const TArray<FUMJoint>& Joints)
{
	FUMJointGroup* JointGroup = new FUMJointGroup();
	JointGroup->Name = Name;
	JointGroup->AddGroups(Groups);
	JointGroup->AddJoints(Joints);
	return *JointGroup;
}

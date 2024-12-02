#include "Animation/UMSequenceStructs.h"

FUMJointKey UUMSequenceHelper::MakeKeyframe(float Time, const FTransform &Transform)
{
	return {Time, Transform};
}

FUMJoint UUMSequenceHelper::MakeJoint(FName NameIn, const FRotatorRange& RangeLimitsIn,
                                     const FUMJointTimeline& SequenceIn)
{ return {NameIn, RangeLimitsIn, SequenceIn}; }

const FUMJointGroup& UUMSequenceHelper::MakeJointGroup(FName Name, const TArray<FUMJointGroup>& Groups,
                                                      const TArray<FUMJoint>& Joints)
{
	FUMJointGroup* JointGroup = new FUMJointGroup();
	JointGroup->Name = Name;
	JointGroup->AddGroups(Groups);
	JointGroup->AddJoints(Joints);
	return *JointGroup;
}

TArray<FUMJointGroup*> FUMJointGroup::AllGroups = TArray<FUMJointGroup*>();

#include "Animation/AnimData/UMSequenceStructs.h"

#include "Animation/UMAnimationProducer.h"

FUMJointKey UUMSequenceHelper::MakeKeyframe(float Time, const FTransform &Transform)
{
	return {Time, Transform};
}

FUMJoint UUMSequenceHelper::MakeJoint(FName NameIn, const FRotatorRange& RangeLimitsIn,
                                     const FUMJointTimeline& SequenceIn)
{ return {NameIn, RangeLimitsIn, SequenceIn}; }

const FUMJointGroup& UUMSequenceHelper::MakeJointGroup(FName Name, TArray<FUMJointGroup>& Groups,
                                                      TArray<FUMJoint>& Joints)
{
	FUMJointGroup* JointGroup = new FUMJointGroup();
	JointGroup->Name = Name;
	for (FUMJointGroup G : Groups) JointGroup->AddGroup(&G);
	JointGroup->AddJoints(Joints);
	return *JointGroup;
}

UAnimSequence* UUMSequenceHelper::BuildSequence(FUMJointGroup JointGroup, USkeletalMesh* SkeletalMesh)
{
	TMap<FName, FUMJointTimeline> Timelines;
	TQueue<FUMJointGroup> Queue;
	Queue.Enqueue(JointGroup);
	while (!Queue.IsEmpty())
	{
		FUMJointGroup Group;
		Queue.Dequeue(Group);
		for (FUMJointGroup* G : Group.Groups)
		{
			Queue.Enqueue(*G);
		}
		for (FUMJoint J : Group.Joints)
		{
			float Scale = J.Timeline.Duration / J.Timeline.Timeline[J.Timeline.Timeline.Num() - 1].Time;
			FUMJointTimeline NewTimeline;
			for (FUMJointKey Key : J.Timeline.Timeline)
			{
				FUMJointKey NewKey;
				NewKey.Time = (Key.Time * Scale) + J.Timeline.StartTime;
				NewKey.Transform = Key.Transform;
				NewTimeline.Timeline.Add(NewKey);
			}
			Timelines.Add(J.Name, NewTimeline);
		}
	}
	return UUMAnimationProducer::CreateSequence(Timelines, SkeletalMesh);
}
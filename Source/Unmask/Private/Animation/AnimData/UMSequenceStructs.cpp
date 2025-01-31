#include "Animation/AnimData/UMSequenceStructs.h"

#include "Animation/UMAnimationProducer.h"

FString FUMJointTimeline::ToString()
{
	FString s = FString("");
	for(int i = 0; i < this->Timeline.Num(); i++)
	{
		auto Keyframe = this->Timeline[i];
		s += FString::SanitizeFloat(Keyframe.Time) + ": " + Keyframe.Transform.ToString();
		if (i < this->Timeline.Num() - 1)
		{
			s += ", ";
		}
	}
	return "Timeline: [" + s + "]";
}

FString FUMJoint::ToString()
{
	return "Joint: " + this->Name.ToString() + ", " + this->CtrlName.ToString() +
		" (" + this->RangeLimits.Min.ToString() + ", " + this->RangeLimits.Max.ToString() + "), " +
		this->Timeline.ToString();
		
}

FString FUMJointGroup::ToString()
{
	auto StringBuilder = new TStringBuilder<1024 * sizeof(FString)>();
	TArray<TPair<FUMJointGroup*, int>> JGStack;
	JGStack.Push(TPair<FUMJointGroup*, int>(this, 0));
	while(!JGStack.IsEmpty())
	{
		TPair<FUMJointGroup*, int> JointGroup_Depth = JGStack.Pop();
		auto JointGroup = JointGroup_Depth.Key;
		int Depth = JointGroup_Depth.Value;
		
		FString TabString = "";
		for(int i = 0; i < Depth; i++)
		{
			TabString += "    ";
		}
		StringBuilder->Append(TabString + JointGroup->Name.ToString() + TEXT("\n"));
		TabString += "  ";
		StringBuilder->Append(TabString + TEXT("Joints: "));
		for (int i = 0; i < JointGroup->Joints.Num(); i++)
		{
			auto& Joint = JointGroup->Joints[i];
			StringBuilder->Append(*Joint.Name.ToString());
			if (i <JointGroup->Joints.Num() - 1)
			{
				StringBuilder->Append(TEXT(", "));
			}else
			{
				StringBuilder->Append(TEXT("\n"));
			}
		}
		StringBuilder->Append(TabString + TEXT("Groups:\n "));
		for (auto Group : JointGroup->Groups)
		{
			JGStack.Push(TPair<FUMJointGroup*, int>(Group, Depth+1));
		}
		
		
	}
	
	return StringBuilder->ToString();
}


FUMJointKey UUMSequenceHelper::MakeKeyframe(float Time, const FTransform &Transform)
{
	return {Time, Transform};
}



const void UUMSequenceHelper::PrintJointGroup(FUMJointGroup Group)
{
	FString out = Group.ToString();
	UE_LOG(LogCore, Display, TEXT("\n%s"), *out);
	if(GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::White,  FString::Printf(TEXT("%s"), *out));
}

FUMJoint UUMSequenceHelper::MakeJoint(FName NameIn, FName CtrlNameIm, const FRotatorRange& RangeLimitsIn,
                                      const FUMJointTimeline& SequenceIn)
{ return {NameIn, CtrlNameIm, RangeLimitsIn, SequenceIn}; }

const FUMJointGroup& UUMSequenceHelper::MakeJointGroup(FName Name, TArray<FUMJointGroup>& Groups,
                                                      TArray<FUMJoint>& Joints)
{
	FUMJointGroup* JointGroup = new FUMJointGroup(Name);
	AddGroups(*JointGroup, Groups);
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
		FUMJointGroup Group {FName()};
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
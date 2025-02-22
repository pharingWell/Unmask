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

FString UUMJointGroup::ToString()
{
	auto StringBuilder = new TStringBuilder<1024 * sizeof(FString)>();
	TArray<TPair<UUMJointGroup*, int>> JGStack;
	JGStack.Push(TPair<UUMJointGroup*, int>(this, 0));
	while(!JGStack.IsEmpty())
	{
		TPair<UUMJointGroup*, int> JointGroup_Depth = JGStack.Pop();
		auto JointGroup = JointGroup_Depth.Key;
		int Depth = JointGroup_Depth.Value;
		
		FString TabString = "";
		for(int i = 0; i < Depth; i++)
		{
			TabString += "    ";
		}
		//UE_LOG(LogCore, Warning, TEXT("TabString length: %d"), TabString.Len());
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
			}
		}
		StringBuilder->Append(TabString + TEXT("Bones: "));
		for (int i = 0; i < JointGroup->Bones.Num(); i++)
		{
			auto& Bone = JointGroup->Bones.Array()[i];
			StringBuilder->Append(*Bone.ToString());
			if (i <JointGroup->Bones.Num() - 1)
			{
				StringBuilder->Append(TEXT(", "));
			}
		}
		StringBuilder->Append(TEXT("\n"));
		StringBuilder->Append(TabString + TEXT("Groups:\n "));
		for (auto Group : JointGroup->Groups)
		{
			JGStack.Push(TPair<UUMJointGroup*, int>(Group, Depth+1));
		}
	}
	
	return StringBuilder->ToString();
}

bool UUMJointGroup::ContainsBone(FName BoneName, bool bIncludeChildren)
{
	if(this->Bones.Contains(BoneName))
	{
		return true;
	}
	if(!bIncludeChildren)
	{
		return false;
	}
	for (UUMJointGroup* JG : this->Groups)
	{
		if (JG->ContainsBone(BoneName, true)) return true;
	}
	return false;
}

FUMJointKey UUMSequenceHelper::MakeKeyframe(float Time, const FTransform &Transform)
{
	return {Time, Transform};
}

const void UUMSequenceHelper::PrintJointGroup(UUMJointGroup* Group)
{
	FString out = Group->ToString();
	UE_LOG(LogCore, Display, TEXT("\n%s"), *out);
	if(GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::White,  FString::Printf(TEXT("%s"), *out));
}

UAnimSequence* UUMSequenceHelper::BuildSequence(UUMJointGroup* JointGroup, USkeletalMesh* SkeletalMesh)
{
	TMap<FName, FUMJointTimeline> Timelines;
	TQueue<UUMJointGroup*> Queue;
	Queue.Enqueue(JointGroup);
	while (!Queue.IsEmpty())
	{
		UUMJointGroup* Group;
		Queue.Dequeue(Group);
		for (UUMJointGroup* G : Group->Groups)
		{
			Queue.Enqueue(G);
		}
		for (FUMJoint J : Group->Joints)
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


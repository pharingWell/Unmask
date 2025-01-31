// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/UMAnimationRecorder.h"
#include "Animation/Skeleton.h"

UUMAnimationRecorder* UUMAnimationRecorder::GetNewAnimationRecorder()
{
	return NewObject<UUMAnimationRecorder>();
}

void UUMAnimationRecorder::InitAnimationRecorder(USkeletalMeshComponent* InSkeletalMeshComponent)
{
	this->SkeletalMeshComponent = InSkeletalMeshComponent;
	
	this->RootGroup.Name = FName("body");
	auto *RootGroupRef = &this->AllGroups.Add(this->RootGroup.Name, this->RootGroup);
	FUMJoint
		Neck(FName("neck_01")),
		Head(FName("head")),
		LeftShoulder(FName("upperarm_l")),
		RightShoulder(FName("upperarm_r")),
		Pelvis(FName("pelvis")),
		Spine01(FName("spine_01")),
		Spine02(FName("spine_02")),
		Spine03(FName("spine_03")),
		LeftClavicle(FName("clavicle_l")),
		RightClavicle(FName("clavicle_r"));

	RootGroupRef->AddJoints({Neck, Head, LeftShoulder, RightShoulder, Pelvis, Spine01, Spine02, Spine03, LeftClavicle, RightClavicle});
	RootGroupRef = this->AllGroups.Find(RootGroup.Name);
	
	FUMJointGroup LeftArm(FName("left arm"));
	auto *LeftArmRef = &AllGroups.Add(LeftArm.Name, LeftArm);
	FUMJoint
		LowerLeftArm(FName("lowerarm_l")),
		LeftHand(FName("hand_l")),
		LeftThumb(FName("thumb_01_l")),
		LeftIndex(FName("index_01_l")),
		LeftMiddle(FName("middle_01_l")),
		LeftRing(FName("ring_01_l")),
		LeftPinky(FName("pinky_01_l"));
	LeftArmRef->AddJoints({LowerLeftArm, LeftHand, LeftThumb, LeftIndex, LeftMiddle, LeftRing, LeftPinky});
	RootGroupRef->AddGroup(LeftArmRef);
	
	FUMJointGroup RightArm(FName("right arm"));
	auto *RightArmRef = &AllGroups.Add(RightArm.Name, RightArm);
	FUMJoint
		LowerRightArm(FName("lowerarm_r")),
		RightHand(FName("hand_r")),
		RightThumb(FName("thumb_01_r")),
		RightIndex(FName("index_01_r")),
		RightMiddle(FName("middle_01_r")),
		RightRing(FName("ring_01_r")),
		RightPinky(FName("pinky_01_r"));
	RightArmRef->AddJoints({LowerRightArm, RightHand, RightThumb, RightIndex, RightMiddle, RightRing, RightPinky});
	RootGroupRef->AddGroup(RightArmRef);
	

	FUMJointGroup LeftLeg(FName("left leg"));
	auto *LeftLegRef = &AllGroups.Add(LeftLeg.Name, LeftLeg);
	FUMJoint
		LeftThigh(FName("thigh_l")),
		LeftCalf(FName("calf_l")),
		LeftFoot(FName("foot_l"));
	LeftLegRef->AddJoints({LeftThigh, LeftFoot, LeftCalf});
	RootGroupRef->AddGroup(LeftLegRef);

	FUMJointGroup RightLeg(FName("right leg"));
	auto *RightLegRef = &AllGroups.Add(RightLeg.Name, RightLeg);
	FUMJoint
		RightThigh(FName("thigh_r")),
		RightCalf(FName("calf_r"));
	RightLegRef->AddJoints({RightThigh, RightCalf});
	RootGroupRef->AddGroup(RightLegRef);
	
	FUMJointGroup RightFoot(FName("foot_r"));
	auto *RightFootRef = &AllGroups.Add(RightFoot.Name, RightFoot);
	RightLegRef->AddGroup(RightFootRef);
	//this->AllGroups.Find(RightLeg.Name)->AddGroup(this->AllGroups.Find(RightFoot.Name));
}

FName UUMAnimationRecorder::GetGroupWithBone(FName BoneName)
{
	FReferenceSkeleton RefSkeleton = SkeletalMeshComponent->GetSkeletalMeshAsset()->GetRefSkeleton();
	const int32 TargetIndex = RefSkeleton.FindBoneIndex(BoneName);
	int32 LowestDepth = 500;
	FName BestGroup = FName();
	for (auto Tuple : AllGroups)
	{
		FUMJointGroup &Group = Tuple.Value;
		for (auto Joint : Group.Joints)
		{
			const int32 BoneIndex = RefSkeleton.FindBoneIndex(Joint.Name);
			int32 Depth = RefSkeleton.GetDepthBetweenBones(TargetIndex, BoneIndex);
			if (Depth != -1 && Depth < LowestDepth)
			{
				LowestDepth = Depth;
				BestGroup = Group.Name;
				if (Depth == 0)
				{
					break;
				}
			}
		}
	}
	return BestGroup;
}

void UUMAnimationRecorder::HideAllButGroup(FName GroupName)
{
	TArray<uint8>& EditableBoneVisibilityStates = SkeletalMeshComponent->GetEditableBoneVisibilityStates();
	for (int BoneIndex = 0; BoneIndex < this->SkeletalMeshComponent->GetBoneSpaceTransforms().Num(); BoneIndex++)
	{
		//this->SkeletalMeshComponent->UnHideBone(BoneIndex);
		if (IsInGroup(this->SkeletalMeshComponent->GetBoneName(BoneIndex), GroupName, false))
		{
			this->SkeletalMeshComponent->HideBone(BoneIndex, PBO_None);
			
			/*if (BoneIndex < EditableBoneVisibilityStates.Num())
			{
				checkSlow ( BoneIndex != INDEX_NONE );
				EditableBoneVisibilityStates[ BoneIndex ] = BVS_ExplicitlyHidden;
				TArray<int32> Children;
				SkeletalMeshComponent->GetSkinnedAsset()->GetSkeleton()->GetChildBones(BoneIndex, Children);
				for (int32 Child : Children)
				{
					if (Child < EditableBoneVisibilityStates.Num())
					{
						checkSlow ( Child != INDEX_NONE );
						EditableBoneVisibilityStates[Child] = BVS_Visible;
						this->SkeletalMeshComponent->UnHideBone(Child);
					}
				}
				SkeletalMeshComponent->RebuildVisibilityArray();
			}*/
		}
	}
}

FString UUMAnimationRecorder::AllGroupsToString()
{
	auto StringBuilder = new TStringBuilder<1024 * sizeof(FString)>();
	StringBuilder->Append(TEXT("["));
	for (auto Tuple : AllGroups)
	{
		auto& Name = Tuple.Key;
		auto& Group = Tuple.Value;
		StringBuilder->Append(Name.ToString() + TEXT(": \n\tJoints: [") );
		for (int i = 0; i < Group.Joints.Num(); i++)
		{
			auto& Joint = Group.Joints[i];
			StringBuilder->Append(*Joint.Name.ToString());
			if (i < Group.Joints.Num() - 1)
			{
				StringBuilder->Append(TEXT(", "));
			}
		}
		StringBuilder->Append(TEXT("]\n\tGroups: ["));
		for (int i = 0; i < Group.Groups.Num(); i++)
		{
			auto& ChildGroup = Group.Groups[i];
			if(ChildGroup->Name.IsValid())
			{
				StringBuilder->Append(ChildGroup->Name.ToString());
			} else
			{
				StringBuilder->Append("*");
			}
			
			if (i < Group.Groups.Num() - 1)
			{
				StringBuilder->Append(TEXT(", "));
			}
		}
		StringBuilder->Append(TEXT("]\n"));
	}
	return StringBuilder->ToString();
}

void UUMAnimationRecorder::PrintAllGroups()
{
	FString out = AllGroupsToString();
	UE_LOG(LogCore, Display, TEXT("\n%s"), *out);
	if(GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::White,  FString::Printf(TEXT("%s"), *out));
}

bool UUMAnimationRecorder::IsInGroup(FName BoneName, FName GroupName, bool bIncludeChildren)
{
	for (FUMJoint J : this->AllGroups.Find(GroupName)->Joints)
	{
		if (J.Name == BoneName) return true;
	}
	if(!bIncludeChildren)
	{
		return false;
	}
	for (FUMJointGroup* JG : this->AllGroups.Find(GroupName)->Groups)
	{
		if (this->IsInGroup(BoneName, JG->Name, true)) return true;
	}
	return false;
}

void UUMAnimationRecorder::LoadTimelines(TMap<FName, FUMJointTimeline> Timelines)
{
	for (TTuple<FName, FUMJointTimeline> Timeline : Timelines)
	{
		for (TTuple<FName, FUMJointGroup> Group : this->AllGroups)
		{
			bool Exit = false;
			for (int i = 0; i < Group.Value.Joints.Num(); i++)
			{
				if (Group.Value.Joints[i].Name == Timeline.Key)
				{
					Exit = true;
					this->AllGroups.Find(Group.Key)->Joints[i].Timeline = Timeline.Value;
					break;
				}
			}
			if (Exit) break;
		}
	}
}

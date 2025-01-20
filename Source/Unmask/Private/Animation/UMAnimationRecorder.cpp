// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/UMAnimationRecorder.h"

UUMAnimationRecorder* UUMAnimationRecorder::GetNewAnimationRecorder()
{
	return NewObject<UUMAnimationRecorder>();
}

void UUMAnimationRecorder::InitAnimationRecorder(USkeletalMeshComponent* InSkeletalMeshComponent)
{
	this->SkeletalMeshComponent = InSkeletalMeshComponent;

	this->RootGroup.Name = FName("body");
	this->AllGroups.Add(this->RootGroup.Name, this->RootGroup);
	FUMJoint Neck, Head, LeftShoulder, RightShoulder, Pelvis, Spine01, Spine02, Spine03, LeftClavicle, RightClavicle;
	Neck.Name = FName("neck_01");
	Head.Name = FName("head");
	Pelvis.Name = FName("pelvis");
	Spine01.Name = FName("spine_01");
	Spine02.Name = FName("spine_02");
	Spine03.Name = FName("spine_03");
	LeftClavicle.Name = FName("clavicle_l");
	RightClavicle.Name = FName("clavicle_r");
	LeftShoulder.Name = FName("upperarm_l");
	RightShoulder.Name = FName("upperarm_r");
	this->AllGroups.Find(this->RootGroup.Name)->AddJoint(Neck);
	this->AllGroups.Find(this->RootGroup.Name)->AddJoint(Head);
	this->AllGroups.Find(this->RootGroup.Name)->AddJoint(LeftShoulder);
	this->AllGroups.Find(this->RootGroup.Name)->AddJoint(RightShoulder);
	this->AllGroups.Find(this->RootGroup.Name)->AddJoint(Pelvis);
	this->AllGroups.Find(this->RootGroup.Name)->AddJoint(Spine01);
	this->AllGroups.Find(this->RootGroup.Name)->AddJoint(Spine02);
	this->AllGroups.Find(this->RootGroup.Name)->AddJoint(Spine03);
	this->AllGroups.Find(this->RootGroup.Name)->AddJoint(LeftClavicle);
	this->AllGroups.Find(this->RootGroup.Name)->AddJoint(RightClavicle);

	FUMJointGroup LeftArm;
	LeftArm.Name = FName("left arm");
	this->AllGroups.Add(LeftArm.Name, LeftArm);
	FUMJoint LowerLeftArm, LeftHand, LeftThumb, LeftIndex, LeftMiddle, LeftRing, LeftPinky;
	LowerLeftArm.Name = FName("lowerarm_l");
	LeftHand.Name = FName("hand_l");
	LeftThumb.Name = FName("thumb_01_l");
	LeftIndex.Name = FName("index_01_l");
	LeftMiddle.Name = FName("middle_01_l");
	LeftRing.Name = FName("ring_01_l");
	LeftPinky.Name = FName("pinky_01_l");
	this->AllGroups.Find(LeftArm.Name)->AddJoint(LowerLeftArm);
	this->AllGroups.Find(LeftArm.Name)->AddJoint(LeftHand);
	this->AllGroups.Find(LeftArm.Name)->AddJoint(LeftThumb);
	this->AllGroups.Find(LeftArm.Name)->AddJoint(LeftIndex);
	this->AllGroups.Find(LeftArm.Name)->AddJoint(LeftMiddle);
	this->AllGroups.Find(LeftArm.Name)->AddJoint(LeftRing);
	this->AllGroups.Find(LeftArm.Name)->AddJoint(LeftPinky);
	this->AllGroups.Find(this->RootGroup.Name)->AddGroup(this->AllGroups.Find(LeftArm.Name));

	FUMJointGroup RightArm;
	RightArm.Name = FName("right arm");
	this->AllGroups.Add(RightArm.Name, RightArm);
	FUMJoint LowerRightArm, RightHand, RightThumb, RightIndex, RightMiddle, RightRing, RightPinky;
	LowerRightArm.Name = FName("lowerarm_r");
	RightHand.Name = FName("hand_r");
	RightThumb.Name = FName("thumb_01_r");
	RightIndex.Name = FName("index_01_r");
	RightMiddle.Name = FName("middle_01_r");
	RightRing.Name = FName("ring_01_r");
	RightPinky.Name = FName("pinky_01_r");
	this->AllGroups.Find(RightArm.Name)->AddJoint(LowerRightArm);
	this->AllGroups.Find(RightArm.Name)->AddJoint(RightHand);
	this->AllGroups.Find(RightArm.Name)->AddJoint(RightThumb);
	this->AllGroups.Find(RightArm.Name)->AddJoint(RightIndex);
	this->AllGroups.Find(RightArm.Name)->AddJoint(RightMiddle);
	this->AllGroups.Find(RightArm.Name)->AddJoint(RightRing);
	this->AllGroups.Find(RightArm.Name)->AddJoint(RightPinky);
	this->AllGroups.Find(this->RootGroup.Name)->AddGroup(this->AllGroups.Find(RightArm.Name));

	FUMJointGroup LeftLeg;
	LeftLeg.Name = FName("left leg");
	auto &LeftLegRef =this->AllGroups.Add(LeftLeg.Name, LeftLeg);
	FUMJoint LeftThigh, LeftCalf, LeftFoot;
	LeftThigh.Name = FName("thigh_l");
	LeftCalf.Name = FName("calf_l");
	LeftFoot.Name = FName("foot_l");
	LeftLegRef.AddJoint(LeftThigh);
	LeftLegRef.AddJoint(LeftCalf);
	LeftLegRef.AddJoint(LeftFoot);
	this->AllGroups.Find(this->RootGroup.Name)->AddGroup(&LeftLegRef);

	FUMJointGroup RightLeg;
	RightLeg.Name = FName("right leg");
	auto &RightLegRef = this->AllGroups.Add(RightLeg.Name, RightLeg);
	FUMJoint RightThigh, RightCalf, RightFoot;
	RightThigh.Name = FName("thigh_r");
	RightCalf.Name = FName("calf_r");
	RightFoot.Name = FName("foot_r");
	RightLegRef.AddJoint(RightThigh);
	RightLegRef.AddJoint(RightCalf);
	RightLegRef.AddJoint(RightFoot);
	this->AllGroups.Find(this->RootGroup.Name)->AddGroup(&RightLegRef);
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
	for (int BoneIndex = 0; BoneIndex < this->SkeletalMeshComponent->GetBoneSpaceTransforms().Num(); BoneIndex++)
	{
		this->SkeletalMeshComponent->UnHideBone(BoneIndex);
		if (!this->IsInGroup(this->SkeletalMeshComponent->GetBoneName(BoneIndex), GroupName))
		{
			this->SkeletalMeshComponent->HideBone(BoneIndex, PBO_None);
			TArray<uint8>& EditableBoneVisibilityStates = SkeletalMeshComponent->GetEditableBoneVisibilityStates();
			if (BoneIndex < EditableBoneVisibilityStates.Num())
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
					}
				}
				SkeletalMeshComponent->RebuildVisibilityArray();
			}
		}
	}
}

bool UUMAnimationRecorder::IsInGroup(FName BoneName, FName GroupName)
{
	for (FUMJoint J : this->AllGroups.Find(GroupName)->Joints)
	{
		if (J.Name == BoneName) return true;
	}
	
	for (FUMJointGroup* JG : this->AllGroups.Find(GroupName)->Groups)
	{
		if (this->IsInGroup(BoneName, JG->Name)) return true;
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

// Fill out your copyright notice in the Description page of Project Settings.


#include "UMMeshPoser.h"

UUMMeshPoser* UUMMeshPoser::GetNewMeshPoser()
{
	return NewObject<UUMMeshPoser>();
}

void UUMMeshPoser::InitMeshPoser(UPoseableMeshComponent* InSkeletalMesh)
{
	this->PoseableMesh = InSkeletalMesh;

	this->RootGroup.Name = FName("body");
	this->AllGroups.Add(this->RootGroup.Name, this->RootGroup);
	FUMJoint Neck, Head, LeftShoulder, RightShoulder;
	Neck.Name = FName("neck_01");
	Head.Name = FName("head");
	LeftShoulder.Name = FName("upperarm_l");
	RightShoulder.Name = FName("upperarm_r");
	this->AllGroups.Find(this->RootGroup.Name)->AddJoint(Neck);
	this->AllGroups.Find(this->RootGroup.Name)->AddJoint(Head);
	this->AllGroups.Find(this->RootGroup.Name)->AddJoint(LeftShoulder);
	this->AllGroups.Find(this->RootGroup.Name)->AddJoint(RightShoulder);

	FUMJointGroup LeftArm;
	LeftArm.Name = FName("left arm");
	this->AllGroups.Add(LeftArm.Name, LeftArm);
	FUMJoint LowerLeftArm, LeftHand;
	LowerLeftArm.Name = FName("lowerarm_l");
	LeftHand.Name = FName("hand_l");
	this->AllGroups.Find(LeftArm.Name)->AddJoint(LowerLeftArm);
	this->AllGroups.Find(LeftArm.Name)->AddJoint(LeftHand);
	this->AllGroups.Find(this->RootGroup.Name)->AddGroup(this->AllGroups.Find(LeftArm.Name));

	FUMJointGroup RightArm;
	RightArm.Name = FName("right arm");
	this->AllGroups.Add(RightArm.Name, RightArm);
	FUMJoint LowerRightArm, RightHand;
	LowerRightArm.Name = FName("lowerarm_r");
	RightHand.Name = FName("hand_r");
	this->AllGroups.Find(RightArm.Name)->AddJoint(LowerRightArm);
	this->AllGroups.Find(RightArm.Name)->AddJoint(RightHand);
	this->AllGroups.Find(this->RootGroup.Name)->AddGroup(this->AllGroups.Find(RightArm.Name));

	FUMJointGroup LeftLeg;
	LeftLeg.Name = FName("left leg");
	auto &LeftLegRef =this->AllGroups.Add(LeftLeg.Name, LeftLeg);
	FUMJoint LeftThigh, LeftCalf;
	LeftThigh.Name = FName("thigh_l");
	LeftCalf.Name = FName("calf_l");
	LeftLegRef.AddJoint(LeftThigh);
	LeftLegRef.AddJoint(LeftCalf);
	this->AllGroups.Find(this->RootGroup.Name)->AddGroup(&LeftLegRef);

	FUMJointGroup RightLeg;
	RightLeg.Name = FName("right leg");
	auto &RightLegRef = this->AllGroups.Add(RightLeg.Name, RightLeg);
	FUMJoint RightThigh, RightCalf;
	RightThigh.Name = FName("thigh_r");
	RightCalf.Name = FName("calf_r");
	RightLegRef.AddJoint(RightThigh);
	RightLegRef.AddJoint(RightCalf);
	this->AllGroups.Find(this->RootGroup.Name)->AddGroup(&RightLegRef);
}

void UUMMeshPoser::SetBoneTransform(FName Bone, FTransform Transform) const
{
	this->PoseableMesh->SetBoneTransformByName(Bone, Transform, EBoneSpaces::Type::ComponentSpace);
}

FName UUMMeshPoser::GetGroupWithBone(FName BoneName)
{
	FReferenceSkeleton RefSkeleton = PoseableMesh->GetSkinnedAsset()->GetRefSkeleton();
	const int32 TargetIndex = RefSkeleton.FindBoneIndex(BoneName);
	int32 LowestDepth = 500;
	FName BestGroup = "body";
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

void UUMMeshPoser::HideAllButGroup(FName GroupName)
{
	for (int i = 0; i < this->PoseableMesh->GetBoneSpaceTransforms().Num(); i++)
	{
		this->PoseableMesh->UnHideBone(i);
		if (!this->IsInGroup(this->PoseableMesh->GetBoneName(i), GroupName))
		{
			//UE_LOG(LogScript, Error, TEXT("%s"), *this->PoseableMesh->GetBoneName(i).ToString())
			this->PoseableMesh->HideBone(i, PBO_None);
		}
	}
}

bool UUMMeshPoser::IsInGroup(FName BoneName, FName GroupName)
{
	for (FUMJoint J : this->AllGroups.Find(GroupName)->Joints)
	{
		if (J.Name.IsEqual(BoneName))
		{
			return true;
		}
	}
	
	for (FUMJointGroup* JG : this->AllGroups.Find(GroupName)->Groups)
	{
		if (this->IsInGroup(BoneName, JG->Name)) return true;
	}

	return false;
}

void UUMMeshPoser::LoadTimelines(TMap<FName, FUMJointTimeline> Timelines)
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

// Fill out your copyright notice in the Description page of Project Settings.


#include "UMMeshPoser.h"

UUMMeshPoser* UUMMeshPoser::GetNewMeshPoser()
{
	return NewObject<UUMMeshPoser>();
}


void UUMMeshPoser::InitMeshPoser(UPoseableMeshComponent* InSkeletalMesh)
{
	this->PoseableMesh = InSkeletalMesh;
	TQueue<FParentChildPair> BoneQueue;
	
	this->JointGroup.AddJoint(FUMJoint(this->PoseableMesh->GetBoneName(0)));
	this->AllGroups.Add(this->JointGroup);
	
	for (int32 B : this->GetChildBones(0))
	{
		BoneQueue.Enqueue(FParentChildPair(0, B));
	}
	
	while (!BoneQueue.IsEmpty())
	{
		FParentChildPair BonePair;
		BoneQueue.Dequeue(BonePair);
		FUMJointGroup JG;
		JG.Name = this->PoseableMesh->GetBoneName(BonePair.ChildIndex);
		this->AllGroups[BonePair.ChildIndex] = JG;
		this->AllGroups[BonePair.ParentIndex].AddJoint(FUMJoint(this->PoseableMesh->GetBoneName(BonePair.ChildIndex)));
		if (this->GetChildBones(BonePair.ChildIndex).Num() != 0) this->AllGroups[BonePair.ParentIndex].AddGroup(this->AllGroups[BonePair.ChildIndex]);
		for (int32 B : this->GetChildBones(BonePair.ChildIndex)) BoneQueue.Enqueue(FParentChildPair(BonePair.ChildIndex, B));
	}

}

TArray<int32> UUMMeshPoser::GetChildBones(int32 ParentBone) const
{
	TArray<int32> ChildBones;
	for (int i = 0; i < this->PoseableMesh->GetNumBones(); i++)
	{
		if (this->PoseableMesh->GetBoneIndex(this->PoseableMesh->GetParentBone(this->PoseableMesh->GetBoneName(i))) == ParentBone)
		{
			ChildBones.Add(i);
		}
	}
	return ChildBones;
}

void UUMMeshPoser::SetBoneTransform(FName Bone, FTransform Transform) const
{
	this->PoseableMesh->SetBoneTransformByName(Bone, Transform, EBoneSpaces::Type::ComponentSpace);
}


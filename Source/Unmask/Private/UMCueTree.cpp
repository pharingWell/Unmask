// Fill out your copyright notice in the Description page of Project Settings.

#include "UMCueTree.h"

#include <random>

float UUMCueTree::RandInRange(float Min, float Max)
{
	return FMath::FRand() * (Max - Min) + Min;
}

FCueTreeNode UUMCueTree::GetRoot()
{
	return this->Nodes[0];
}

UUMCueTree::UUMCueTree()
{
	this->Ranges = TMap<FName, FBoneRotatorRange>();
	this->SkeletalMesh = nullptr;
}

UUMCueTree::UUMCueTree(USkeletalMesh *SkeletalMesh, const TMap<FName, FBoneRotatorRange> Ranges, const int AvgChildren, const int MinDepth, const int MaxDepth, const int Frames, const float PlayLength)
{
	this->Ranges = Ranges;
	this->SkeletalMesh = SkeletalMesh;
	TQueue<int> ToBeExpanded;
	FCueTreeNode Root;
	Root.Depth = 0;
	Nodes.Add(Root);
	ToBeExpanded.Enqueue(0);
	while (!ToBeExpanded.IsEmpty())
	{
		int ParentNodeIndex;
		ToBeExpanded.Dequeue(ParentNodeIndex);
		if (Nodes[ParentNodeIndex].Depth >= MaxDepth) continue;
		int ChildrenCount = FMath::RandRange(0, 2 * AvgChildren); // make this a normal distribution
		if (ChildrenCount < 1 and Nodes[ParentNodeIndex].Depth < MinDepth) ChildrenCount = 1;
		for (int i = 0; i < ChildrenCount; i++)
		{
			FCueTreeNode ChildNode;
			ChildNode.Depth = Nodes[ParentNodeIndex].Depth + 1;
			ChildNode.Animation = GenerateAnimation(Frames, PlayLength);
			int ChildNodeIndex = Nodes.Add(ChildNode);
			Nodes[ParentNodeIndex].Children.Add(ChildNodeIndex);
			ToBeExpanded.Enqueue(ChildNodeIndex);
		}
	}
}

UAnimSequence *UUMCueTree::GenerateAnimation(const int Frames, const float PlayLength)
{
	float DeltaTime = PlayLength / Frames;
	TMap<FName, FUMJointSequence> Joints;
	
	for (TTuple<FName, FBoneRotatorRange> Range : this->Ranges)
	{
		FUMJointSequence JointSequence;
		const int Index = this->SkeletalMesh->GetSkeleton()->GetReferenceSkeleton().FindBoneIndex(Range.Key);
		JointSequence.JointSequence.Add(FUMKeyFrame(0, this->SkeletalMesh->GetSkeleton()->GetReferenceSkeleton().GetRefBonePose()[Index]));
		Joints.Add(Range.Key, JointSequence);
	}
	
	for (int T = 1; T < Frames; T++)
	{
		for (TTuple<FName, FBoneRotatorRange> Range : this->Ranges)
		{
			const FRotator Max = Range.Value.RotatorMax;
			const FRotator Min = Range.Value.RotatorMin;
			
			const float MaxPitchMovement = DeltaTime * (Max.Pitch - Min.Pitch);
			const float MaxRollMovement = DeltaTime * (Max.Roll - Min.Roll);
			const float MaxYawMovement = DeltaTime * (Max.Yaw - Min.Yaw);

			const FRotator LastPose = FRotator(Joints.Find(Range.Key)->JointSequence.Last().Transform.Rotator());

			const float NewJointPitch = FMath::Clamp(RandInRange(-1, 1) * MaxPitchMovement + LastPose.Pitch, Min.Pitch, Max.Pitch);
			const float NewJointRoll = FMath::Clamp(RandInRange(-1, 1) * MaxRollMovement + LastPose.Roll, Min.Roll, Max.Roll);
			const float NewJointYaw = FMath::Clamp(RandInRange(-1, 1) * MaxYawMovement + LastPose.Yaw, Min.Yaw, Max.Yaw);
			
			Joints.Find(Range.Key)->JointSequence.Add(FUMKeyFrame(T * DeltaTime, FTransform(FRotator(NewJointPitch, NewJointYaw, NewJointRoll))));
		}
	}

	return UUMAnimationProducer::CreateSequence(Joints, this->SkeletalMesh);
}

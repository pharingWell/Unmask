// Fill out your copyright notice in the Description page of Project Settings.

#include "UMCueTree.h"

#include <random>

float UUMCueTree::RandInRange(float Min, float Max)
{
	return FMath::FRand() * (Max - Min) + Min;
}

UUMCueTree* UUMCueTree::CreateMVPTree(USkeletalMesh* Mesh)
{
	TMap<FName, FRotatorRange> Ranges;
	Ranges.Add(FName("upperarm_r"), FRotatorRange(FRotator(-90, 0, 0), FRotator(90, 0, 0)));
	Ranges.Add(FName("lowerarm_r"), FRotatorRange(FRotator(0, 0, -90), FRotator(180, 0, 90)));
	Ranges.Add(FName("hand_r"), FRotatorRange(FRotator(-90, -90, -90), FRotator(90, 90, 90)));
	
	UUMCueTree* Tree = NewObject<UUMCueTree>();
	Tree->InitCueTree(Mesh, Ranges, 1, 1, 3, 27, 1);
	return Tree;
}

FUMCueTreeNode UUMCueTree::GetRoot()
{
	return this->Nodes[0];
}

UUMCueTree::UUMCueTree()
{
	this->Ranges = TMap<FName, FRotatorRange>();
	this->SkeletalMesh = nullptr;
}

void UUMCueTree::InitCueTree(USkeletalMesh *InitSkeletalMesh, const TMap<FName, FRotatorRange> InitRanges,
	const int AvgChildren, const int MinDepth, const int MaxDepth, const int Frames, const float PlayLength)
{
	this->Ranges = InitRanges;
	this->SkeletalMesh = InitSkeletalMesh;
	TQueue<int> ToBeExpanded;
	FUMCueTreeNode Root;
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
			FUMCueTreeNode ChildNode;
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
	TMap<FName, FUMJointTimeline> Joints;
	
	for (TTuple<FName, FRotatorRange> Range : this->Ranges)
	{
		FUMJointTimeline JointTimeline;
		const int Index = this->SkeletalMesh->GetSkeleton()->GetReferenceSkeleton().FindBoneIndex(Range.Key);
		JointTimeline.Timeline.Add(FUMJointKey(0, this->SkeletalMesh->GetSkeleton()->GetReferenceSkeleton().GetRefBonePose()[Index]));
		Joints.Add(Range.Key, JointTimeline);
	}
	
	for (int T = 1; T < Frames; T++)
	{
		for (auto [Name, Range] : this->Ranges)
		{
			const FRotator Max = Range.Max;
			const FRotator Min = Range.Min;
			
			const float MaxPitchMovement = DeltaTime * (Max.Pitch - Min.Pitch);
			const float MaxRollMovement = DeltaTime * (Max.Roll - Min.Roll);
			const float MaxYawMovement = DeltaTime * (Max.Yaw - Min.Yaw);

			const FRotator LastPose = FRotator(Joints.Find(Name)->Timeline.Last().Transform.Rotator());

			const float NewJointPitch = FMath::Clamp(RandInRange(-1, 1) * MaxPitchMovement + LastPose.Pitch, Min.Pitch, Max.Pitch);
			const float NewJointRoll = FMath::Clamp(RandInRange(-1, 1) * MaxRollMovement + LastPose.Roll, Min.Roll, Max.Roll);
			const float NewJointYaw = FMath::Clamp(RandInRange(-1, 1) * MaxYawMovement + LastPose.Yaw, Min.Yaw, Max.Yaw);
			
			Joints.Find(Name)->Timeline.Add(FUMJointKey(T * DeltaTime, FTransform(FRotator(NewJointPitch, NewJointYaw, NewJointRoll))));
		}
	}

	return UUMAnimationProducer::CreateSequence(Joints, this->SkeletalMesh);
}

// Fill out your copyright notice in the Description page of Project Settings.

#include "UMCueTree.h"

#include <random>

float UUMCueTree::RandInRange(const float Min, const float Max)
{
	return ((rand() % 100) / 100.0f * (Max - Min) + Min);
}

FCueTreeNode *UUMCueTree::GetRoot()
{
	return &(this->Nodes[0]);
}

UUMCueTree::UUMCueTree(USkeletalMesh *SkeletalMesh, const TMap<FName, FBoneRotatorRange> *Ranges, const int AvgChildren, const int MinDepth, const int MaxDepth, const int Frames, const float PlayLength)
{
	this->SkeletalMesh = SkeletalMesh;
	std::default_random_engine Generator;
	std::normal_distribution<int> Distribution(AvgChildren, AvgChildren / 3.0);
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
		int ChildrenCount = Distribution(Generator);
		if (ChildrenCount < 1 and Nodes[ParentNodeIndex].Depth < MinDepth) ChildrenCount = 1;
		for (int i = 0; i < ChildrenCount; i++)
		{
			FCueTreeNode ChildNode;
			ChildNode.Depth = Nodes[ParentNodeIndex].Depth + 1;
			ChildNode.Animation = GenerateAnimation(SkeletalMesh, Ranges, Frames, PlayLength);
			int ChildNodeIndex = Nodes.Add(ChildNode);
			Nodes[ParentNodeIndex].Children.Add(ChildNodeIndex);
			ToBeExpanded.Enqueue(ChildNodeIndex);
		}
	}
}

UAnimSequence *UUMCueTree::GenerateAnimation(USkeletalMesh *SkeletalMesh, const TMap<FName, FBoneRotatorRange> *Ranges, const int Frames, const float PlayLength)
{
	float DeltaTime = PlayLength / Frames;
	TMap<FName, FUMJointSequence> Joints;
	
	for (TTuple<FName, FBoneRotatorRange> Range : Ranges)
	{
		FUMJointSequence JointSequence;
		const int Index = SkeletalMesh->GetSkeleton()->GetReferenceSkeleton().FindBoneIndex(Range.Key);
		JointSequence.JointSequence.Add(FUMKeyFrame(0, SkeletalMesh->GetSkeleton()->GetReferenceSkeleton().GetRefBonePose()[Index]));
		Joints.Add(Range.Key, JointSequence);
	}
	
	for (int T = 1; T < Frames; T++)
	{
		for (TTuple<FName, FBoneRotatorRange> Range : Ranges)
		{
			FRotator Max = Range.Value.RotatorMax;
			FRotator Min = Range.Value.RotatorMin;
			
			const float MaxPitchMovement = DeltaTime * (Max.Pitch() - Min.Pitch());
			const float MaxRollMovement = DeltaTime * (Max.Roll() - Min.Roll());
			const float MaxYawMovement = DeltaTime * (Max.Yaw() - Min.Yaw());

			FRotator LastPose = FRotator(Joints.Find(Range.Key)->JointSequence.Last().Transform.Rotator());

			const float NewJointPitch = std::min(Max.Pitch(), std::max(Min.Pitch(), RandInRange(-1, 1) * MaxPitchMovement + LastPose.Pitch()));
			const float NewJointRoll = std::min(Max.Roll(), std::max(Min.Roll(), RandInRange(-1, 1) * MaxRollMovement + LastPose.Roll()));
			const float NewJointYaw = std::min(Max.Yaw(), std::max(Min.Yaw(), RandInRange(-1, 1) * MaxYawMovement + LastPose.Yaw()));
			
			Joints.Find(Range.Key)->JointSequence.Add(FUMKeyFrame(T * DeltaTime, FTransform(FRotator(NewJointPitch, NewJointYaw, NewJointRoll))));
		}
	}

	return UUMAnimationProducer::CreateSequence(Joints, SkeletalMesh);
}

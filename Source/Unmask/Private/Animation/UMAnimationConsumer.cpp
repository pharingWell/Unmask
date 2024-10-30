// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/UMAnimationConsumer.h"

TArray<FBoneIndexType> UUMAnimationConsumer::BoneIndexTypeArrayOfSize(int size)
{
	TArray<FBoneIndexType> BoneIndexType;
	for (int32 i = 0; i < size; i++)
	{
		BoneIndexType.Add(i);
	}
	return BoneIndexType;
}

float UUMAnimationConsumer::SequenceDifference(UAnimSequence *X, UAnimSequence *Y, float Fidelity)
{
	FMemMark Mark(FMemStack::Get());
	
	float Distance = 0;

	USkeleton* XSkeleton = X->GetSkeleton();
	FBoneContainer XBoneContainer = FBoneContainer(BoneIndexTypeArrayOfSize(XSkeleton->GetReferenceSkeleton().GetNum()), UE::Anim::FCurveFilterSettings(), *XSkeleton);
	
	FCompactPose XPose = FCompactPose();
	XPose.SetBoneContainer(&XBoneContainer);
	FBlendedCurve XCurve = FBlendedCurve();
	UE::Anim::FStackAttributeContainer XAttributes;
	FAnimationPoseData XData = FAnimationPoseData(XPose, XCurve, XAttributes);

	USkeleton* YSkeleton = Y->GetSkeleton();
	FBoneContainer YBoneContainer = FBoneContainer(BoneIndexTypeArrayOfSize(YSkeleton->GetReferenceSkeleton().GetNum()), UE::Anim::FCurveFilterSettings(), *YSkeleton);
	
	FCompactPose YPose = FCompactPose();
	YPose.SetBoneContainer(&YBoneContainer);
	FBlendedCurve YCurve = FBlendedCurve();
	UE::Anim::FStackAttributeContainer YAttributes;
	FAnimationPoseData YData = FAnimationPoseData(YPose, YCurve, YAttributes);
	
	for (double t = 0; t < std::max(X->GetPlayLength(), Y->GetPlayLength()); t += Fidelity)
	{
		X->GetBonePose(XData, FAnimExtractContext(t));
		Y->GetBonePose(YData, FAnimExtractContext(t));
		
		TArray<FTransform> XBonePoses = reinterpret_cast<const TArray<FTransform>&>(XPose.GetBones());
		TArray<FTransform> YBonePoses = reinterpret_cast<const TArray<FTransform>&>(YPose.GetBones());

		for (int j = 0; j < XBonePoses.Num(); j++)
		{
			FRotator XRotator = XBonePoses[j].Rotator();
			FRotator YRotator = YBonePoses[j].Rotator();
			Distance += Fidelity*sqrt(pow(XRotator.Pitch-YRotator.Pitch, 2) + pow(XRotator.Roll-YRotator.Roll, 2) + pow(XRotator.Yaw-YRotator.Yaw, 2));
		}
	}
	Mark.Pop();
	return Distance;
}
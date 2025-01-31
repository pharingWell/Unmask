// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/UMAnimationConsumer.h"

TArray<FBoneIndexType> UUMAnimationConsumer::BoneIndexTypeArrayOfSize(const int Size)
{
	TArray<FBoneIndexType> BoneIndexTypeArray;
	for (int32 i = 0; i < Size; i++)
	{
		BoneIndexTypeArray.Add(i);
	}
	return BoneIndexTypeArray;
}

float UUMAnimationConsumer::SequenceDifference(UAnimSequence *X, UAnimSequence *Y, const float Fidelity, OUT float &Scale)
{
	FMemMark Mark(FMemStack::Get());
	
	float Difference = 0;

	if (X == nullptr || Y == nullptr)
	{
		FString ProblemAsset = "";
		if (X == nullptr)
		{
			ProblemAsset += "X";
		}
		if (Y == nullptr)
		{
			ProblemAsset += "Y";
		}
		UE_LOG(LogScript, Error, TEXT("UMAnimationConsumer: Called on missing sequence %s."), ToCStr(ProblemAsset));
		return -99999;
	}
	
	USkeleton* XSkeleton = X->GetSkeleton();
	USkeleton* YSkeleton = Y->GetSkeleton();
	
	FBoneContainer XBoneContainer = FBoneContainer(BoneIndexTypeArrayOfSize(XSkeleton->GetReferenceSkeleton().GetNum()), UE::Anim::FCurveFilterSettings(), *XSkeleton);
	FBoneContainer YBoneContainer = FBoneContainer(BoneIndexTypeArrayOfSize(YSkeleton->GetReferenceSkeleton().GetNum()), UE::Anim::FCurveFilterSettings(), *YSkeleton);
	
	FCompactPose XPose = FCompactPose();
	XPose.SetBoneContainer(&XBoneContainer);
	FBlendedCurve XCurve = FBlendedCurve();
	UE::Anim::FStackAttributeContainer XAttributes;
	FAnimationPoseData XData = FAnimationPoseData(XPose, XCurve, XAttributes);
	
	FCompactPose YPose = FCompactPose();
	YPose.SetBoneContainer(&YBoneContainer);
	FBlendedCurve YCurve = FBlendedCurve();
	UE::Anim::FStackAttributeContainer YAttributes;
	FAnimationPoseData YData = FAnimationPoseData(YPose, YCurve, YAttributes);

	Scale = Y->GetPlayLength() / X->GetPlayLength();
	
	for (double t = 0; t < X->GetPlayLength(); t += Fidelity)
	{
		X->GetBonePose(XData, FAnimExtractContext(t));
		Y->GetBonePose(YData, FAnimExtractContext(t * Scale));
		
		TArray<FTransform> XBonePoses = reinterpret_cast<const TArray<FTransform>&>(XPose.GetBones());
		TArray<FTransform> YBonePoses = reinterpret_cast<const TArray<FTransform>&>(YPose.GetBones());

		for (int j = 0; j < XBonePoses.Num(); j++)
		{
			FRotator XRotator = XBonePoses[j].Rotator();
			FRotator YRotator = YBonePoses[j].Rotator();
			Difference += Fidelity*sqrt(pow(XRotator.Pitch-YRotator.Pitch, 2) + pow(XRotator.Roll-YRotator.Roll, 2) + pow(XRotator.Yaw-YRotator.Yaw, 2));
		}
	}
	Mark.Pop();
	return Difference;
}
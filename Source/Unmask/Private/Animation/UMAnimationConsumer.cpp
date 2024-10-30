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
	
	USkeleton* XSkeleton = X->GetSkeleton();
	FBoneContainer XBoneContainer = FBoneContainer(BoneIndexTypeArrayOfSize(XSkeleton->GetReferenceSkeleton().GetNum()), UE::Anim::FCurveFilterSettings(), *XSkeleton);
	
	FCompactPose XPose = FCompactPose();

	XPose.SetBoneContainer(&XBoneContainer);
	FBlendedCurve XCurve = FBlendedCurve();
	UE::Anim::FStackAttributeContainer XAttributes = UE::Anim::FStackAttributeContainer();
	FAnimationPoseData XData = FAnimationPoseData(XPose, XCurve, XAttributes);

	USkeleton* YSkeleton = Y->GetSkeleton();
	FBoneContainer YBoneContainer = FBoneContainer(BoneIndexTypeArrayOfSize(YSkeleton->GetReferenceSkeleton().GetNum()), UE::Anim::FCurveFilterSettings(), *YSkeleton);
	
	FCompactPose YPose = FCompactPose();
	YPose.SetBoneContainer(&YBoneContainer);
	FBlendedCurve YCurve = FBlendedCurve();
	UE::Anim::FStackAttributeContainer YAttributes = UE::Anim::FStackAttributeContainer();
	FAnimationPoseData YData = FAnimationPoseData(YPose, YCurve, YAttributes);
	
#if WITH_EDITOR
#define LOCTEXT_NAMESPACE "Initialize Data Model"
	IAnimationDataController& XController = X->GetController();
	XController.InitializeModel();
	IAnimationDataController& YController = Y->GetController();
	YController.InitializeModel();
#endif
#undef LOCTEXT
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
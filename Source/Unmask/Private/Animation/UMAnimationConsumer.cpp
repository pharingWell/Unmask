// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/UMAnimationConsumer.h"

static TArray<FBoneIndexType> BoneTreeToBoneIndexTypeArray(TArray<int32> BoneTree)
{
	TArray<FBoneIndexType> BoneIndexType;
	for (int32 i = 0; i < BoneTree.Num(); i++)
	{
		BoneIndexType.Add(BoneTree[i]);
	}
	return BoneIndexType;
}

float UMAnimationConsumer::SequenceDifference(UAnimSequence *X, UAnimSequence *Y, float Fidelity)
{
	FMemMark Mark(FMemStack::Get());
	
	float Distance = 0;

	USkeleton* XSkeleton = X->GetSkeleton();
	TArray<int32> XBones;
	XSkeleton->GetChildBones(0, XBones);
	FBoneContainer XBoneContainer = FBoneContainer(BoneTreeToBoneIndexTypeArray(XBones), UE::Anim::FCurveFilterSettings(), *XSkeleton);
	
	FCompactPose XPose = FCompactPose();
	XPose.SetBoneContainer(&XBoneContainer);
	FBlendedCurve XCurve = FBlendedCurve();
	UE::Anim::FStackAttributeContainer XAttributes = UE::Anim::FStackAttributeContainer();
	FAnimationPoseData XData = FAnimationPoseData(XPose, XCurve, XAttributes);

	USkeleton* YSkeleton = Y->GetSkeleton();
	TArray<int32> YBones;
	YSkeleton->GetChildBones(0, YBones);
	FBoneContainer YBoneContainer = FBoneContainer(BoneTreeToBoneIndexTypeArray(YBones), UE::Anim::FCurveFilterSettings(), *YSkeleton);
	
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
	return Distance;
}
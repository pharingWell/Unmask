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

float UMAnimationConsumer::MontageDifference(UAnimMontage *X, UAnimMontage *Y, float Fidelity)
{
	float Distance = 0;
	for (double t = 0; t < std::max(X->CalculateSequenceLength(), Y->CalculateSequenceLength()); t += Fidelity)
	{
		TArray<FSlotAnimationTrack> XTrack = X->SlotAnimTracks;
		TArray<FSlotAnimationTrack> YTrack = Y->SlotAnimTracks;

		for (int i = 0; i < XTrack.Num(); i++)
		{
			FAnimSegment XSegment = *(XTrack[i].AnimTrack.GetSegmentAtTime(t));
			FAnimSegment YSegment = *(YTrack[i].AnimTrack.GetSegmentAtTime(t));

			float TF = t;

			UAnimSequenceBase *XSequence = (XSegment.GetAnimationData(t - XSegment.AnimStartTime, TF));
			UAnimSequenceBase *YSequence = (YSegment.GetAnimationData(t - YSegment.AnimStartTime, TF));

			FCompactPose XPose, YPose;
			FBlendedCurve XCurve, YCurve;
			UE::Anim::FStackAttributeContainer XContainer, YContainer;
			FAnimationPoseData XData = FAnimationPoseData(XPose, XCurve, XContainer);
			FAnimationPoseData YData = FAnimationPoseData(YPose, YCurve, YContainer);
			FAnimExtractContext XOutContext = FAnimExtractContext(t - XSegment.AnimStartTime, false, {}, false);
			FAnimExtractContext YOutContext = FAnimExtractContext(t - YSegment.AnimStartTime, false, {}, false);

#if WITH_EDITOR
#define LOCTEXT_NAMESPACE "Initialize Data Model"
			IAnimationDataController& XController = XSequence->GetController();
			XController.InitializeModel();
			IAnimationDataController& YController = YSequence->GetController();
			YController.InitializeModel();
#endif
#undef LOCTEXT
			
			XSequence->GetAnimationPose(XData, XOutContext);
			YSequence->GetAnimationPose(YData, YOutContext);

			TArray<FTransform> XBones = XData.GetPose().GetBoneContainer().GetRefPoseArray();
			TArray<FTransform> YBones = YData.GetPose().GetBoneContainer().GetRefPoseArray();

			for (int j = 0; j < XBones.Num(); j++)
			{
				FRotator XRotator = XBones[j].Rotator();
				FRotator YRotator = YBones[j].Rotator();
				Distance += sqrt(pow(XRotator.Pitch - YRotator.Pitch, 2) + pow(XRotator.Roll - YRotator.Roll, 2) + pow(XRotator.Yaw - YRotator.Yaw, 2));
			}
		}
	}
	return Distance;
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
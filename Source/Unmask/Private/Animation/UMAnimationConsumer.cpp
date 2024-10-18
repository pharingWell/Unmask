// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/UMAnimationConsumer.h"

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
	float Distance = 0;
	for (double t = 0; t < std::max(X->GetPlayLength(), Y->GetPlayLength()); t += Fidelity)
	{
		FCompactPose XPose = FCompactPose();//, YPose = {};
		FBlendedCurve XCurve = FBlendedCurve();//, YCurve = {};
		UE::Anim::FStackAttributeContainer XContainer = UE::Anim::FStackAttributeContainer();//, YContainer = {};
		FAnimationPoseData XData = FAnimationPoseData(XPose, XCurve, XContainer);
		//FAnimationPoseData YData = FAnimationPoseData(YPose, YCurve, YContainer);
		FAnimExtractContext XOutContext = FAnimExtractContext(t, false, {}, false);
		//FAnimExtractContext YOutContext = FAnimExtractContext(t, false, {}, false);
		
#if WITH_EDITOR
#define LOCTEXT_NAMESPACE "Initialize Data Model"
		IAnimationDataController& XController = X->GetController();
		XController.InitializeModel();
		//IAnimationDataController& YController = Y->GetController();
		//YController.InitializeModel();
#endif
#undef LOCTEXT
		
		X->GetAnimationPose(XData, XOutContext);
		//Y->GetAnimationPose(YData, YOutContext);

		XData.GetPose();
		//TArray<FTransform> XBones = XPose.GetBoneContainer().GetRefPoseArray();
		//TArray<FTransform> YBones = YData.GetPose().GetBoneContainer().GetRefPoseArray();

		/*for (int j = 0; j < XBones.Num(); j++)
		{
			FRotator XRotator = XBones[j].Rotator();
			//FRotator YRotator = YBones[j].Rotator();
			Distance += sqrt(pow(XRotator.Pitch, 2) + pow(XRotator.Roll, 2) + pow(XRotator.Yaw, 2));
		}*/
	}
	return Distance;
}

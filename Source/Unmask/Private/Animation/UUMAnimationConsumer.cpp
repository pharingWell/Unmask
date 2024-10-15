// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/UUMAnimationConsumer.h"

float UUMAnimationConsumer::Distance(UAnimMontage *X, UAnimMontage *Y, float Fidelity)
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

// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/UMAnimationRecorder.h"

#include "Animation/UMAnimationProducer.h"

FRecorderData UUMAnimationRecorder::InitRecorderData(USkeletalMeshComponent* Component, int Frames, int FrameRate)
{
	TMap<FName, FUMJointTimeline> Joints;
	for (int i = 0; i < Component->GetBoneSpaceTransforms().Num(); i++)
	{
		Joints.Add(Component->GetBoneName(i), FUMJointTimeline());
	}
	FRecorderData OutData;
	OutData.Component = Component;
	OutData.Frames = Frames;
	OutData.FrameRate = FrameRate;
	OutData.Joints = Joints;
	return OutData;
}

void UUMAnimationRecorder::RecordFrame(FRecorderData Data)
{
	if (Data.FrameRate == 0) return;
	if (Data.CurrentFrame % (Data.Frames / Data.FrameRate) == 0 && Data.CurrentFrame < Data.Frames)
	{
		TArray<FTransform> Transforms = Data.Component->GetBoneSpaceTransforms();
		for (int i = 0; i < Transforms.Num(); i++)
		{
			Data.Joints.Find(Data.Component->GetBoneName(i))->Timeline.Add(FUMJointKey(0, Transforms[i]));
		}
	}
	Data.CurrentFrame++;
}

UAnimSequence* UUMAnimationRecorder::ProducePlayback(FRecorderData Data)
{
	return UUMAnimationProducer::CreateSequence(Data.Joints, Data.Component->GetSkeletalMeshAsset());
}
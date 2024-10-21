// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/UMAnimationRecorder.h"

#include "Animation/UMAnimationProducer.h"

FRecorderData UUMAnimationRecorder::InitRecorderData(USkeletalMeshComponent* Component, int Frames, int FrameRate)
{
	TMap<FName, FUMJointSequence> Joints;
	for (int i = 0; i < Component->GetBoneSpaceTransforms().Num(); i++)
	{
		Joints.Add(Component->GetBoneName(i), FUMJointSequence());
	}
	FRecorderData OutData;
	OutData.Component = Component;
	OutData.Frames = Frames;
	OutData.FrameRate = FrameRate;
	OutData.Joints = Joints;
	return OutData;
}

void UUMAnimationRecorder::RecordFrame(FRecorderData data)
{
	if (data.CurrentFrame % (data.Frames / data.FrameRate) == 0 && data.CurrentFrame < data.Frames)
	{
		TArray<FTransform> Transforms = data.Component->GetBoneSpaceTransforms();
		for (int i = 0; i < Transforms.Num(); i++)
		{
			data.Joints.Find(data.Component->GetBoneName(i))->JointSequence.Add(FUMKeyFrame(0, Transforms[i]));
		}
	}
	data.CurrentFrame++;
}

UAnimSequence* UUMAnimationRecorder::ProducePlayback(FRecorderData Data)
{
	return UUMAnimationProducer::CreateSequence(Data.Joints, Data.Component->GetSkeletalMeshAsset());
}
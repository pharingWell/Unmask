// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UMSequenceStructs.h"
#include "UObject/Object.h"
#include "UMAnimationRecorder.generated.h"

USTRUCT(Blueprintable, BlueprintType)
struct FRecorderData
{
	GENERATED_BODY()
	
	TMap<FName, FUMJointTimeline> Joints;
	USkeletalMeshComponent* Component;
	int Frames, FrameRate;
	int CurrentFrame = 0;
};
/**
 * 
 */
UCLASS()
class UNMASK_API UUMAnimationRecorder : public UObject
{
	GENERATED_BODY()

	public:
		UFUNCTION(BlueprintCallable, Category = "Animation|Recording")
		static FRecorderData InitRecorderData(USkeletalMeshComponent* Component, int Frames, int Framerate);
		UFUNCTION(BlueprintCallable, Category = "Animation|Recording")
		static void RecordFrame(FRecorderData Data);
		UFUNCTION(BlueprintCallable, Category = "Animation|Recording")
		static UAnimSequence* ProducePlayback(FRecorderData Data);
};

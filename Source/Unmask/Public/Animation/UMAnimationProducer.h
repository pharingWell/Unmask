// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimData/UMSequenceStructs.h"
#include "UObject/Object.h"
#include "UMAnimationProducer.generated.h"


DECLARE_LOG_CATEGORY_EXTERN(LogAnimProducer, Verbose, All);

static int32 AnimationNumber = 0;

UCLASS()
class UNMASK_API UUMAnimationProducer : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Animation|Sequencer")
	static UAnimSequence* CreateSequence(TMap<FName, FUMJointTimeline> JointTimelineMap, USkeletalMesh* AnimatedObject);
	static constexpr int32 ResampleRate = 30;
};

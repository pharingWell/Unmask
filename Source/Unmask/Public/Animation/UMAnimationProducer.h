// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UMSequenceStructs.h"
#include "UObject/Object.h"
#include "UMAnimationProducer.generated.h"


UCLASS()
class UNMASK_API UUMAnimationProducer : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Animation|Sequencer")
	static UAnimSequence* CreateSequence(const TMap<FName, FUMJointTimeline>& JointTimeline, USkeletalMesh* AnimatedObject);
};

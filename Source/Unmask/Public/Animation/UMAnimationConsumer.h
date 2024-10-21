// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UMAnimationConsumer.generated.h"

/**
 * 
 */
UCLASS()
class UNMASK_API UUMAnimationConsumer : public UObject
{
	GENERATED_BODY()

	public:
		UFUNCTION(BlueprintCallable, Category = "Animation|Math")
		static float SequenceDifference(UAnimSequence *X, UAnimSequence *Y, float Fidelity);
};
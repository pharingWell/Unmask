// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UUMAnimationConsumer.generated.h"

/**
 * 
 */
UCLASS()
class UNMASK_API UUMAnimationConsumer : public UObject
{
	GENERATED_BODY()

	public:
	    UFUNCTION(BlueprintCallable, Category = "AnimMath")
		static float Distance(UAnimMontage *X, UAnimMontage *Y, float Fidelity);
};
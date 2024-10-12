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
		static float Distance(UAnimMontage x, UAnimMontage y, float fidelity);
};
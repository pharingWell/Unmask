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

	protected:
		static TArray<FBoneIndexType> BoneIndexTypeArrayOfSize(int Size);

	public:
		
	UFUNCTION(BlueprintCallable, Category = "Math|AnimSequence")
	static float SequenceDifference(UAnimSequence *X, UAnimSequence *Y, float Fidelity, float &Scale);
};
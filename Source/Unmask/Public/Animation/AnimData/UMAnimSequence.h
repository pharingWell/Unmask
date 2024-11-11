// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UMAnimDataController.h"
#include "Animation/AnimSequence.h"
#include "UMAnimSequence.generated.h"

/**
 * 
 */
UCLASS()
class UNMASK_API UUMAnimSequence : public UAnimSequence
{
	GENERATED_BODY()
public:
	void CorrectModel();
	virtual void PostLoad() override;
	IAnimationDataController& GetUMController();
private:
	bool bModelCorrected = false;
	
};

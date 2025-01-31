// Fill out your copyright notice in the Description page of Project Settings.
#pragma once


#if WITH_EDITOR
#include "CoreMinimal.h"
#include "Editor/UnrealEdEngine.h"
#include "MyEdEngine.generated.h"

/**
 * 
 */
UCLASS()
class UMyEdEngine : public UUnrealEdEngine
{
	GENERATED_BODY()
	virtual void Init(IEngineLoop* InEngineLoop) override;
};
#endif

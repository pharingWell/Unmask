// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UMPathLocation.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class UNMASK_API UUMPathLocation : public UObject
{
public:
	GENERATED_BODY()
	UPROPERTY()
	FVector Location;
	UPROPERTY()
	TArray<UUMPathLocation*> NextLocations;
	UUMPathLocation()
	{
		this->Location = FVector();
	}
	UFUNCTION(BlueprintCallable)
	UUMPathLocation* GetRandomNext();
	UFUNCTION(BlueprintCallable)
	FVector GetLocation(double Z);
};

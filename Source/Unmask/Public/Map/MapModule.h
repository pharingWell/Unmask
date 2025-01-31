// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "MapModule.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class UNMASK_API UMapModule : public UObject
{
	GENERATED_BODY()
	UPROPERTY()
	UUMPathLocation* NorthStart;
	UPROPERTY()
	UUMPathLocation* SouthStart;
	UPROPERTY()
	UUMPathLocation* EastStart;
	UPROPERTY()
	UUMPathLocation* WestStart;
public:
	UMapModule() {}
	UFUNCTION(BlueprintCallable)
	static UMapModule* InitTestingMapModule();
	UFUNCTION(BlueprintCallable)
	UUMPathLocation* GetNorthStart() { return NorthStart; }
	UFUNCTION(BlueprintCallable)
	UUMPathLocation* GetSouthStart() { return SouthStart; }
	UFUNCTION(BlueprintCallable)
	UUMPathLocation* GetEastStart() { return EastStart; }
	UFUNCTION(BlueprintCallable)
	UUMPathLocation* GetWestStart() { return WestStart; }
	//UFUNCTION(BlueprintCallable)
	//FVector GetRelativeLocation(UUMPathLocation* Location, FVector Center, double Orientation);
};

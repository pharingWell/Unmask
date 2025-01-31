// Fill out your copyright notice in the Description page of Project Settings.


#include "Map/UMPathLocation.h"

UUMPathLocation* UUMPathLocation::GetRandomNext()
{
	if (this->NextLocations.Num() == 0)
	{
		return this;
	}
	int Min = 0;
	int Max = this->NextLocations.Num();
	return this->NextLocations[FMath::Floor(FMath::FRand() * (Max - Min) + Min)];
}

FVector UUMPathLocation::GetLocation(double Z)
{
	FVector Out = this->Location;
	Out.Z = Z;
	return Out;
}
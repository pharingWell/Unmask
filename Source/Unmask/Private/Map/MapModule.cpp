// Fill out your copyright notice in the Description page of Project Settings.


#include "Map/MapModule.h"
#include "Map/UMPathLocation.h"

UMapModule* UMapModule::InitTestingMapModule()
{
	UMapModule* out = NewObject<UMapModule>();
	FVector L1 = FVector(1360, 1880, -50);
	FVector L2 = FVector(1860, 2380, -50);
	FVector L3 = FVector(860, 2380, -50);
	FVector L4 = FVector(1360, 2880, -50);
	UUMPathLocation* PL4 = NewObject<UUMPathLocation>();
	PL4->Location = L4;
	UUMPathLocation* PL3 = NewObject<UUMPathLocation>();
	PL3->Location = L3;
	PL3->NextLocations.Add(PL4);
	UUMPathLocation* PL2 = NewObject<UUMPathLocation>();
	PL2->Location = L2;
	PL2->NextLocations.Add(PL4);
	UUMPathLocation* PL1 = NewObject<UUMPathLocation>();
	PL1->Location = L1;
	PL1->NextLocations.Add(PL2);
	PL1->NextLocations.Add(PL3);
	out->SouthStart = PL1;
	return out;
}
/*
FVector UMapModule::GetRelativeLocation(UUMPathLocation* Location, FVector Center, double Orientation)
{
	return FVector(Location.Location.X * FMath::Cos(Orientation) - Location.Location.Y * FMath::Sin(Orientation) + Center.X,
		Location.Location.X * FMath::Sin(Orientation) + Location.Location.Y * FMath::Cos(Orientation) + Center.Y,
		Center.Z);	
}
*/
// Copyright Epic Games, Inc. All Rights Reserved.

#include "UncoverGameMode.h"
#include "UncoverCharacter.h"
#include "UObject/ConstructorHelpers.h"

AUncoverGameMode::AUncoverGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

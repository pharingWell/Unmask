// Copyright Epic Games, Inc. All Rights Reserved.

#include "UnmaskGameMode.h"
#include "UnmaskCharacter.h"
#include "UObject/ConstructorHelpers.h"

AUnmaskGameMode::AUnmaskGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

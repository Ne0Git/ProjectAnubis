// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProjectAnubisGameMode.h"
#include "ProjectAnubisCharacter.h"
#include "UObject/ConstructorHelpers.h"

AProjectAnubisGameMode::AProjectAnubisGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

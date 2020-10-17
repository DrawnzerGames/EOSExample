// Copyright Epic Games, Inc. All Rights Reserved.

#include "EOSGameMode.h"
#include "EOSCharacter.h"
#include "UObject/ConstructorHelpers.h"

AEOSGameMode::AEOSGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

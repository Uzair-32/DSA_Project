// Copyright Epic Games, Inc. All Rights Reserved.

#include "project_goldfishGameMode.h"
#include "FpsCharacter.h"
#include "UObject/ConstructorHelpers.h"
/**
 * Constructor
 * Sets up the default pawn class for this GameMode.
 */
Aproject_goldfishGameMode::Aproject_goldfishGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	// Assign the found Blueprint class as the default pawn
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}

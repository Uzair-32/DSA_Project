// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "project_goldfishGameMode.generated.h"
/**
 * Aproject_goldfishGameMode:
 * 
 * Custom GameMode class for the project_goldfish game.
 * Inherits from AGameModeBase, which provides basic game rules, 
 * player spawning, and match flow.
 * 
 * Currently, this class only provides a constructor but can be extended 
 * to override game start, player spawning, scoring rules, etc.
 */
UCLASS(minimalapi)
class Aproject_goldfishGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	// constructor
	Aproject_goldfishGameMode();
};




// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FpsPlayerController.generated.h"

class UInputMappingContext;

/**
 * AFpsPlayerController
 *
 * Custom PlayerController class responsible for managing
 * player-specific input and high-level control logic.
 *
 * This class integrates Unreal Engine's Enhanced Input System
 * by applying an Input Mapping Context at runtime.
 */
UCLASS()
class PROJECT_GOLDFISH_API AFpsPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:
	/**
	 * Input Mapping Context assigned to this player.
	 * 
	 * Defines all input actions and their bindings
	 * (movement, camera, shooting, etc.).
	 * Applied during BeginPlay.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputMappingContext* InputMappingContext;

	// Begin Actor interface
protected:
	/**
	 * Called when the game starts or when the controller is spawned.
	 * Used to initialize input mapping contexts and controller-specific logic.
	 */
	virtual void BeginPlay() override;

	// End Actor interface
};

// Copyright Epic Games, Inc. All Rights Reserved.


#include "FpsPlayerController.h"
#include "EnhancedInputSubsystems.h"
/**
 * Called when the PlayerController is initialized at runtime.
 * Responsible for setting up input bindings using the Enhanced Input system.
 */
void AFpsPlayerController::BeginPlay()
{
	// Call base class implementation to ensure standard PlayerController setup
	Super::BeginPlay();


	// Access the Enhanced Input subsystem for the local player
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		// Add the Input Mapping Context with priority 0
		// This registers all action bindings (movement, look, shooting, etc.)
		Subsystem->AddMappingContext(InputMappingContext, 0);
		
		// Debug log to confirm BeginPlay was executed
		UE_LOG(LogTemp, Warning, TEXT("BeginPlay"));
	}
}
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "FpsCharacter.h"
#include "TP_PickUpComponent.generated.h"

// Delegate declaration for pickup events
// Broadcasts when a character picks up this component
// Parameter: the character that picked it up
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPickUp, AFpsCharacter*, PickUpCharacter);

/**
 * UTP_PickUpComponent:
 * A Sphere Component that can be attached to items in the world to allow
 * players to pick them up.
 * 
 * Features:
 * - Broadcasts a delegate when a character overlaps it.
 * - Can be easily attached to actors and used in Blueprints.
 */
UCLASS(Blueprintable, BlueprintType, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROJECT_GOLDFISH_API UTP_PickUpComponent : public USphereComponent
{
	GENERATED_BODY()

public:
	// Event delegate for pickup notifications.
	// Other classes (like UI or inventory) can bind to this to react to pickups.
	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnPickUp OnPickUp;

	/** Constructor: Sets default values for the component */
	UTP_PickUpComponent();

protected:
	/** Called when the game starts or when spawned */
	virtual void BeginPlay() override;

	/**
	 * Called automatically when another actor overlaps this sphere.
	 * 
	 * @param OverlappedComponent - The sphere component itself
	 * @param OtherActor - The actor that began overlap
	 * @param OtherComp - The specific component of the other actor
	 * @param OtherBodyIndex - Body index of the other component
	 * @param bFromSweep - Whether this was a sweep-based overlap
	 * @param SweepResult - Detailed hit information from the sweep
	 */
	UFUNCTION()
	void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
	                          UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, 
	                          const FHitResult& SweepResult);
};

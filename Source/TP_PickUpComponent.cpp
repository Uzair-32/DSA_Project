// Copyright Epic Games, Inc. All Rights Reserved.

#include "TP_PickUpComponent.h"

/**
 * Constructor
 * Sets default values for this component
 */
UTP_PickUpComponent::UTP_PickUpComponent()
{
    // Set the radius of the pickup sphere
    // This determines how close the player needs to be to trigger the pickup
    SphereRadius = 32.f;
}

/**
 * Called when the game starts or when spawned
 */
void UTP_PickUpComponent::BeginPlay()
{
    Super::BeginPlay();

    // Register our overlap event callback
    // This will call OnSphereBeginOverlap whenever something enters the sphere
    OnComponentBeginOverlap.AddDynamic(this, &UTP_PickUpComponent::OnSphereBeginOverlap);
}

/**
 * Called when another actor overlaps this sphere component
 *
 * @param OverlappedComponent - The sphere component itself
 * @param OtherActor - The actor that began overlap
 * @param OtherComp - The specific component of the other actor
 * @param OtherBodyIndex - Body index of the other component
 * @param bFromSweep - Whether this was triggered by a sweep
 * @param SweepResult - Details about the hit
 */
void UTP_PickUpComponent::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor,
	UPrimitiveComponent* OtherComp,int32 OtherBodyIndex,bool bFromSweep,const FHitResult& SweepResult)
{
    // Check if the overlapping actor is a first-person character
    AFpsCharacter* pCharacter = Cast<AFpsCharacter>(OtherActor);
    if (pCharacter != nullptr)
    {
        // Broadcast the pickup event to any listeners
        // For example, UI or inventory systems can respond to this
        OnPickUp.Broadcast(pCharacter);

        // Prevent the overlap event from triggering again
        // This effectively disables the pickup once it has been collected
        OnComponentBeginOverlap.RemoveAll(this);
    }
}

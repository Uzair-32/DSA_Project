// Copyright Epic Games, Inc. All Rights Reserved.

#include "project_goldfishProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"

/**
 * Constructor
 * Sets up the collision, movement, and lifespan for the projectile.
 */
Aproject_goldfishProjectile::Aproject_goldfishProjectile() 
{
	// Create a sphere component to represent collision
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f); // Small radius for projectile collision
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile"); // Use built-in Projectile collision profile

	// Bind OnHit function to collision event
	CollisionComp->OnComponentHit.AddDynamic(this, &Aproject_goldfishProjectile::OnHit);

	// Prevent players or AI characters from walking on the projectile
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set collision component as the root component
	RootComponent = CollisionComp;

	// Create the projectile movement component
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp; // Movement affects the collision component
	ProjectileMovement->InitialSpeed = 3000.f; // Initial speed of the projectile
	ProjectileMovement->MaxSpeed = 3000.f; // Maximum speed
	ProjectileMovement->bRotationFollowsVelocity = true; // Rotate projectile to follow velocity
	ProjectileMovement->bShouldBounce = true; // Enable bouncing on impact

	// Set default lifespan so the projectile destroys itself after 3 seconds
	InitialLifeSpan = 3.0f;
}

/**
 * Called when the projectile hits another actor.
 * 
 * @param HitComp - The component that registered the hit
 * @param OtherActor - The actor that was hit
 * @param OtherComp - The specific component of the other actor
 * @param NormalImpulse - Impulse applied due to the hit
 * @param Hit - Detailed hit information
 */
void Aproject_goldfishProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Only apply physics impulse and destroy projectile if hitting a valid physics-simulated component
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr) && OtherComp->IsSimulatingPhysics())
	{
		// Apply a force proportional to projectile velocity at the location of the hit
		OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());

		// Destroy the projectile to avoid further collisions
		Destroy();
	}
}

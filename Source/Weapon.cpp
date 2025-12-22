// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon.h"
/**
 * Constructor
 * Sets default values for this actor.
 */
AWeapon::AWeapon()
{
 	// Enable ticking every frame. Useful if the weapon needs dynamic behavior like rotating or updating effects.
	PrimaryActorTick.bCanEverTick = true;

	// Create and initialize the static mesh component for the weapon
	// This will visually represent the weapon in the world
	m_pMesh = CreateDefaultSubobject<UStaticMeshComponent>("Weapon Mesh");

	// Create and initialize the weapon component which handles firing, reloading, ammo, and effects
	PWeaponComponent = CreateDefaultSubobject<UTP_WeaponComponent>("Weapon Component");

	// Attach the mesh to the weapon component so that all transforms follow the component
	// Ensures that when the weapon component moves or rotates, the mesh moves with it
	m_pMesh->SetupAttachment(PWeaponComponent);
}

/**
 * Called when the game starts or when spawned.
 * Can be used to initialize runtime properties or bind events.
 */
void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	// Currently empty, but can be used for runtime initialization
}

/**
 * Called every frame.
 * Useful for continuous updates like animating weapon effects, tracking cooldowns, etc.
 */
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

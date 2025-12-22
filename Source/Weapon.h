// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TP_WeaponComponent.h"       // Include for weapon functionality
#include "NiagaraFunctionLibrary.h"  // Include for particle effects (optional)
#include "Weapon.generated.h"

/**
 * AWeapon:
 * 
 * A weapon actor that contains a mesh and a weapon component.
 * The weapon component handles firing, reloading, ammo, and effects.
 */
UCLASS()
class PROJECT_GOLDFISH_API AWeapon : public AActor
{
	GENERATED_BODY()

	// The visual representation of the weapon in the world
	UPROPERTY(VisibleDefaultsOnly, Category = "Mesh")
	UStaticMeshComponent* m_pMesh;
	
public:
	// The weapon component attached to this weapon actor
	// Handles shooting, reloading, and ammo management
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	UTP_WeaponComponent* PWeaponComponent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Constructor: sets default values
	AWeapon();

	// Called every frame
	virtual void Tick(float DeltaTime) override;
};

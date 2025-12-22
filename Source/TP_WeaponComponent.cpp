// Copyright Epic Games, Inc. All Rights Reserved.


#include "TP_WeaponComponent.h"
#include "FpsCharacter.h"
#include "project_goldfishProjectile.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "HealthInterface.h"

// Sets default values for this component's properties
UTP_WeaponComponent::UTP_WeaponComponent()
{
	// Initialize ammo counts: fill magazine and reserve ammo
	m_iCurrentAmmo = m_iClipSize;
	m_iHolsteredAmmo = m_iMaxAmmo;
}

/**
 * Fire the weapon:
 * - Performs line trace to detect hits
 * - Applies damage to hit targets implementing IHealthInterface
 * - Plays sound and visual effects
 * - Decrements ammo
 */
void UTP_WeaponComponent::Fire()
{
	// Ensure the weapon has a valid character and controller
	if (m_pCharacter == nullptr || m_pCharacter->GetController() == nullptr)
	{
		return;
	}

	APlayerController* playerController = Cast<APlayerController>(m_pCharacter->GetController());
	const FVector playerLocation = m_pCharacter->GetActorLocation();
	const FRotator spawnRotation = playerController->PlayerCameraManager->GetCameraRotation();
	
	UWorld* const world = GetWorld();
	if (world != nullptr)
	{
		// Check clip is not empty.
		if (m_iCurrentAmmo <= 0)
		{    // Play empty clip sound if no ammo
			UGameplayStatics::PlaySoundAtLocation(world, EmptyClipSound, playerLocation);
			return;
		}
		
		// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
		const FVector spawnLocation = playerLocation + spawnRotation.RotateVector(MuzzleOffset);

		// Set up Query params.
		FCollisionQueryParams queryParams;
		queryParams.AddIgnoredActor(playerController->GetPawn());
		queryParams.AddIgnoredActor(GetOwner());

		// Setup the hit result.
		FHitResult outHit;

        // Perform line trace for hits along camera forward vector
		world->LineTraceSingleByChannel(outHit, spawnLocation, spawnLocation + (spawnRotation.Vector() * 3000), ECollisionChannel::ECC_Pawn, queryParams);
		DrawDebugLine(world, spawnLocation, spawnLocation + (spawnRotation.Vector() * 3000), FColor::Red, false, 1.0f, 5, 10.0f);

		// Try to hit a damageable object.
		IHealthInterface* pHealth = Cast<IHealthInterface>(outHit.GetActor());
		if (pHealth != nullptr)
		{
			// Deal damage
			float damageDealt = GetShotDamage();
			pHealth->ReceiveDamage(damageDealt);
		}
		else
		{
			// Play environmental sounds.
			auto sound = EnvironmentalSounds[FMath::RandRange(0, EnvironmentalSounds.Num() - 1)];
			UGameplayStatics::PlaySoundAtLocation(world, sound, playerLocation);
		}

        // Reduce ammo count
		m_iCurrentAmmo = m_iCurrentAmmo - 1;
	}
	
    // Play firing sound if assigned
	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, playerLocation);
	}

    // Spawn muzzle flash particle system if assigned
	if (MuzzleFlashPfx != nullptr)
	{
		// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
		const FVector spawnLocation = GetOwner()->GetActorLocation() + spawnRotation.RotateVector(MuzzleOffset);

		UNiagaraFunctionLibrary::SpawnSystemAtLocation(world, MuzzleFlashPfx, spawnLocation, spawnRotation);
	}
}
/**
 * Reload weapon:
 * - Fills the magazine with ammo from holstered reserve
 */
void UTP_WeaponComponent::Reload()
{
	// Fill the clip with available ammo.
	int reloadAmount = m_iClipSize - m_iCurrentAmmo;
	reloadAmount = UKismetMathLibrary::Min(reloadAmount, m_iHolsteredAmmo);
	m_iHolsteredAmmo -= reloadAmount;
	m_iCurrentAmmo += reloadAmount;
}

/**
 * Check if the weapon can reload:
 * - Cannot reload if clip is full or no reserve ammo
 */
bool UTP_WeaponComponent::CanReload()
{
	bool clipFull = m_iCurrentAmmo == m_iClipSize;
	bool ammoAvailable = m_iHolsteredAmmo > 0;
	if (clipFull || !ammoAvailable)
		return false;

	return true;
}
/**
 * Attach the weapon to a character:
 * - Attaches mesh to character's weapon socket
 * - Binds input actions using Enhanced Input
 */
void UT
void UTP_WeaponComponent::AttachWeapon(AFpsCharacter* TargetCharacter)
{
	m_pCharacter = TargetCharacter;
	if (m_pCharacter == nullptr)
		return;

	// Attach the weapon to the First Person Character
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	USkeletalMeshComponent* pCharacterMesh = m_pCharacter->GetMesh1P();
	AttachToComponent(pCharacterMesh, AttachmentRules, FName(TEXT("WeaponSocket")));

	// Set up action bindings
	if (APlayerController* playerController = Cast<APlayerController>(m_pCharacter->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(playerController->GetLocalPlayer()))
		{
			// Set the priority of the mapping to 1, so that it overrides the Jump action with the Fire action when using touch input
			Subsystem->AddMappingContext(FireMappingContext, 1);
		}

		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(playerController->InputComponent))
		{
			// Bind the fire action to this component's Fire function
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &UTP_WeaponComponent::Fire);
		}
	}
}
/** Returns a random damage value within the weapon's damage range */
float UTP_WeaponComponent::GetShotDamage()
{
    return FMath::RandRange(m_fDamagePerShotMin, m_fDamagePerShotMax);
}
/** Returns ammo in holstered reserve */
int UTP_WeaponComponent::GetHolsteredAmmoAvailable()
{
return m_iHolsteredAmmo;
}
/** Returns ammo currently in magazine */
int UTP_WeaponComponent::GetCurrentMagazineAmmo()
{
    return m_iCurrentAmmo;
}
/**
 * Clean up when component ends play:
 * - Removes input mapping context to avoid dangling bindings
 */
void UTP_WeaponComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (m_pCharacter == nullptr)
	{
		return;
	}

	if (APlayerController* playerController = Cast<APlayerController>(m_pCharacter->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(playerController->GetLocalPlayer()))
		{
			Subsystem->RemoveMappingContext(FireMappingContext);
		}
	}
}
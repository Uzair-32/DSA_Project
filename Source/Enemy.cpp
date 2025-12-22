// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Kismet/GameplayStatics.h"
#include "FpsCharacter.h"


// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	// Initialize health and store the spawn location for pooling logic.
	FHealth = FInitialHealth;
	m_vSpawnLocation = GetActorLocation();
	
	// Bind the animation end event to our handler function.
	// This allows us to know when the Death animation finishes to remove the body.
	GetMesh()->GetAnimInstance()->OnMontageEnded.AddDynamic(this, &AEnemy::HandleOnMontageEnded);
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* pPlayerInputComponent)
{
	Super::SetupPlayerInputComponent(pPlayerInputComponent);
}

void AEnemy::ReceiveDamage(int iAmount)
{
	if (FHealth <= 0)
		return; // Prevent logic from running if the enemy is already dead/dying.

	// Apply damage.
	FHealth -= iAmount;
	
	// Broadcast event for UI or other listeners (e.g. health bars).
	OnEnemyDamaged.Broadcast((float)iAmount);

	// Play reaction to damage sfx at the enemy's location.
	UWorld* pWorld = GetWorld();
	UGameplayStatics::PlaySoundAtLocation(pWorld, PDamagedSound, GetActorLocation());

	// Reward points to the player for damaging an enemy.
	// Casts to the specific AFpsCharacter to access the Stats component.
	AFpsCharacter* pPlayerCharacter = Cast<AFpsCharacter>(pWorld->GetFirstPlayerController()->GetCharacter());
	if (pPlayerCharacter)
	{
		pPlayerCharacter->Stats->AddPoints(IPointsPerHitTaken);
	}

	// Check if health has depleted to trigger death.
	if (FHealth <= 0)
	{
		Die();
	}
}

void AEnemy::RecoverHealth(int iAmount)
{
	FHealth += iAmount;
}

UAnimMontage* AEnemy::GetAttackMontage()
{
    return PAttackMontage;
}

float AEnemy::GetAttackRange()
{
    return FAttackRange;
}

float AEnemy::GetBaseSpeed()
{
	return FBaseSpeed;
}

void AEnemy::Attack()
{
	UWorld* pWorld = GetWorld();

	// Play the attack animation.
	UAnimInstance* pAnimInstance = GetMesh()->GetAnimInstance();
	if (pAnimInstance != nullptr)
	{
		// Prevent attacking if the enemy is currently playing the death animation.
		if (pAnimInstance->Montage_IsPlaying(PDeathMontage))
			return; 

		pAnimInstance->Montage_Play(PAttackMontage);
	}

	// Play a random attack sfx from the array.
	if (PAttackSounds.Num() > 0)
	{
		USoundBase* pAttackSound = PAttackSounds[FMath::RandRange(0, PAttackSounds.Num() - 1)];
		UGameplayStatics::PlaySoundAtLocation(pWorld, pAttackSound, GetActorLocation());
	}

	// Deal damage to the player.
	// Finds the player character and calls their ReceiveDamage interface/method.
	AFpsCharacter* pPlayer = Cast<AFpsCharacter>(UGameplayStatics::GetPlayerCharacter(pWorld, 0));
	if (pPlayer)
	{
		pPlayer->ReceiveDamage(FAttackDamage);
	}
}

void AEnemy::Die()
{
	// Play the death animation.
	UAnimInstance* pAnimInstance = GetMesh()->GetAnimInstance();
	if (pAnimInstance != nullptr)
	{
		// Ensure we don't restart the montage if it's already playing.
		if (!pAnimInstance->Montage_IsPlaying(PDeathMontage))
		{
			pAnimInstance->Montage_Play(PDeathMontage);
		}
	}

	// Play a random death sfx from the array.
	UWorld* pWorld = GetWorld();
	if (PDeathSounds.Num() > 0)
	{
		USoundBase* pDeathSound = PDeathSounds[FMath::RandRange(0, PDeathSounds.Num() - 1)];
		UGameplayStatics::PlaySoundAtLocation(pWorld, pDeathSound, GetActorLocation());
	}

	// Reward points for killing an enemy (Bonus points for the kill).
	AFpsCharacter* pPlayerCharacter = Cast<AFpsCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter());
	if (pPlayerCharacter)
	{
		pPlayerCharacter->Stats->AddPoints(IPointsFromDeath);
	}
}

void AEnemy::ReturnToPool()
{
	// Execute the OnEnemyKilled delegate to notify the spawner this enemy is free.
	OnEnemyKilled.ExecuteIfBound();
	OnEnemyKilled.Clear(); // Clear binding to prevent stale references.

	// Return enemy back to the original spawn pool location.
	TeleportTo(m_vSpawnLocation, GetActorRotation());

	// Reset health and internal state so the enemy is ready for reuse.
	FHealth = FInitialHealth;
	BInArena = false;
}


void AEnemy::HandleOnMontageEnded(UAnimMontage* pMontage, bool bWasInterrupted)
{
	// Check if the montage that just finished was a Death animation.
	// We rely on the animation asset name containing "Death".
	if (pMontage && pMontage->GetName().Contains("Death"))
	{
		// Once the death animation finishes, we recycle the enemy.
		ReturnToPool();
	}
}
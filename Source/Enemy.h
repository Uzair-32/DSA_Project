// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HealthInterface.h"
#include "Enemy.generated.h"

// Delegate for notifying when this specific enemy is killed (single binding).
DECLARE_DYNAMIC_DELEGATE(FOnEnemyKilled);

// Multicast delegate for notifying UI or other systems when damage is taken (multiple bindings).
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemyDamaged, float, fDamageTaken);

/**
 * AEnemy:
 * Manages an enemy Actor.
 * Inherits from ACharacter for movement/physics and IHealthInterface for damage handling.
 */
UCLASS()
class PROJECT_GOLDFISH_API AEnemy : public ACharacter, public IHealthInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties.
	AEnemy();

	// Triggers when the enemy has died. Used by the spawner/manager to track active enemies.
	FOnEnemyKilled OnEnemyKilled;

	// Triggers when the enemy takes damage or receives health. Usable in Blueprints.
	UPROPERTY(BlueprintAssignable)
	FOnEnemyDamaged OnEnemyDamaged;

	// Called every frame.
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input.
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// IHealthInterface Implementation:
	// Deal damage to the enemy.
	virtual void ReceiveDamage(int amount) override;
	// Restore health.
	virtual void RecoverHealth(int amount) override;

	// Perform an attack logic (Animation, Sound, Damage dealing).
	void Attack();

	/*
	Getter functions.
	*/
	// Return the enemy's attack montage asset.
	UAnimMontage* GetAttackMontage();
	// Return the enemy's attack range.
	float GetAttackRange();
	// Return the enemy's base speed stat.
	float GetBaseSpeed();

	// True if the enemy is active in the arena (spawned and fighting).
	bool BInArena = false;

protected:
	// Called when the game starts or when spawned.
	virtual void BeginPlay() override;

	// Animation Montage to play when attacking.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Animation")
	UAnimMontage* PAttackMontage;

	// Animation Montage to play when dying.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Animation")
	UAnimMontage* PDeathMontage;

	// Array of potential sounds to play on attack (randomly selected).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sound")
	TArray<USoundBase*> PAttackSounds;

	// Array of potential sounds to play on death (randomly selected).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sound")
	TArray<USoundBase*> PDeathSounds;

	// Sound to play when the enemy takes damage.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sound")
	USoundBase* PDamagedSound;

	// Starting health value.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float FInitialHealth = 30.0f;

	// Amount of damage this enemy deals to the player.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float FAttackDamage = 33.4f;

	// Distance required to attack the player.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float FAttackRange = 100.0f;

	// Movement speed.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float FBaseSpeed = 0.0f;

	// Score points awarded to player per hit landed on this enemy.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float IPointsPerHitTaken = 10;

	// Score points awarded to player for killing this enemy.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float IPointsFromDeath = 150;

	// Current health at runtime.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float FHealth = 1.0f;

	// Widget class for displaying damage numbers (UI).
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<class UUserWidget> m_cDamageIndicator;

private:
	// Location to return to when pooled.
	FVector m_vSpawnLocation;

	// Trigger the death sequence (Anim, Sound, Scoring).
	void Die();
	
	// Reset the enemy state and teleport back to spawn (Object Pooling).
	void ReturnToPool();

	// Callback bound to the AnimInstance to detect when animations finish.
	UFUNCTION()
	void HandleOnMontageEnded(UAnimMontage* pMontage, bool bWasInterrupted);
};
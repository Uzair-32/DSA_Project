// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyDirector.generated.h"

// Multicast delegate to broadcast when the wave number changes (e.g., for UI updates).
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWaveChanged, int, iWave);

/**
 * AEnemyDirector:
 * Manages the flow of gameplay waves, enemy spawning, object pooling, and difficulty scaling.
 */
UCLASS()
class PROJECT_GOLDFISH_API AEnemyDirector : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemyDirector();

	// Event delegate functions

	// Event triggered when the wave index updates.
	UPROPERTY(BlueprintAssignable)
	FOnWaveChanged OnWaveChanged;

	// End of event delegate functions

	// Called every frame to manage spawning logic.
	virtual void Tick(float DeltaTime) override;

	// List of locations where enemies can spawn.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Management")
	TArray<FVector> PSpawnLocations;

	// Number of enemies to spawn in the very first wave.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Waves")
	int IInitialWaveSpawnCount = 5;

	// Current wave number.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Waves")
	int ICurrentWave = 0;

	// The wave number at which enemy count stops growing linearly (caps the difficulty curve).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Waves")
	int IFinalGrowthWave = 50;

	// The absolute maximum total enemies that can appear in a single wave.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Waves")
	int IMaxEnemiesInWave = 666;

	// The absolute cap on how many enemies can exist in the level simultaneously (performance/gameplay limit).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Waves")
	int IMaxEnemyArenaCapacity = 50;

	// The wave number at which the arena capacity reaches its maximum.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Waves")
	int IWaveMaxEnemyArenaCapacityReached = 22;

	// The current limit on concurrent enemies in the arena (scales up as waves progress).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Waves")
	int IMaxEnemiesInArena = 5;

	// Delay time between waves.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Waves")
	float FSecondsBeforeWaveStarts = 4.0f;

	// Delay time after clearing a wave before the intermission starts.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Waves")
	float FSecondsBeforeWaveEnds = 4.0f;

	// Callback function bound to enemy death events.
	UFUNCTION()
	void ConfirmEnemyKilled();

	// Force an update on the UI elements.
	UFUNCTION(BlueprintCallable, Category="HUD")
	void RefreshUI();

	// Main logic to check if enemies should be moved from the pool to the arena.
	void AttemptSpawnEnemies();
	// Calculates and updates the total number of enemies for the current wave.
	int UpdateWaveSize();
	// Calculates and updates the concurrent enemy limit for the current wave.
	int UpdateEnemyArenaCapacity();
	// Updates difficulty parameters (speeds, counts) for the new wave.
	void UpdateWaveParameters();
	// Initiates the next wave.
	void NextWave();
	// Ends the current wave (when all enemies are dead).
	void EndWave();
	// Helper to trigger spawn attempts.
	void SpawnMoreEnemies();
	
	// Helper: Get all active enemies currently fighting.
	TArray<AActor*> GetAllEnemiesInArena();
	// Helper: Get all inactive enemies waiting in the object pool.
	TArray<AActor*> GetAllEnemiesInPool();
	
	// Updates enemy movement speeds based on current difficulty scaling.
	void ModifyWaveSpeeds();

	// Total enemies remaining to be killed in this wave.
	int ICurrentWaveSize = 0;
	// Number of enemies killed so far in this wave.
	int IWaveKills = 0;
	// Master list of all enemy actors managed by this director.
	TArray<AActor*> PEnemies;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Callback after the end-wave delay.
	void EndWaveDelayedCallback();
	// Callback after the start-wave delay.
	void NextWaveDelayedCallback();

	// The final maximum speed an enemy can move at (hard cap).
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Speed")
	float m_fGlobalFinalMaxWalkSpeed = 400.0f;

	// The final minimum speed an enemy can move at (hard cap).
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Speed")
	float m_fGlobalFinalMinWalkSpeed = 200.0f;

	// The current maximum speed enemies are assigned this wave.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Speed")
	float m_fGlobalMaxWalkSpeed = 120.0f;

	// The current minimum speed enemies are assigned this wave.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Speed")
	float m_fGlobalMinWalkSpeed = 70.0f;

	// Handle for managing wave timers.
	FTimerHandle pTimerHandleCurrent;
	
private:
	// Flag indicating if we are in the break between waves.
	bool m_bWaveIntermission;

	// Clears the active timer safely.
	void ClearCurrentTimer();
};
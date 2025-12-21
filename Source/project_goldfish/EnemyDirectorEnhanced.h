// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CustomHashMap.h"
#include "CustomPriorityQueue.h"
#include "Quadtree.h"
#include "SortingAlgorithms.h"
#include "SearchAlgorithms.h"
#include "EnemyDirectorEnhanced.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWaveChanged, int, iWave);

/**
 * Enhanced Enemy Priority for sorting
 */
USTRUCT(BlueprintType)
struct FEnemyPriority
{
    GENERATED_BODY()

    UPROPERTY()
    int32 EnemyID;

    UPROPERTY()
    float Priority;

    UPROPERTY()
    float DistanceToPlayer;

    FEnemyPriority()
        : EnemyID(-1)
        , Priority(0.0f)
        , DistanceToPlayer(0.0f)
    {
    }

    FEnemyPriority(int32 InID, float InPriority, float InDistance)
        : EnemyID(InID)
        , Priority(InPriority)
        , DistanceToPlayer(InDistance)
    {
    }

    bool operator<(const FEnemyPriority& Other) const
    {
        return Priority < Other.Priority;
    }

    bool operator==(const FEnemyPriority& Other) const
    {
        return EnemyID == Other.EnemyID;
    }
};

/**
 * AEnemyDirectorEnhanced:
 * Enhanced enemy director using custom data structures and algorithms.
 * 
 * Data Structures Used:
 * - CustomHashMap: Fast enemy lookup by ID - O(1) average
 * - CustomPriorityQueue: Enemy targeting prioritization - O(log n) insert/remove
 * - Quadtree: Spatial partitioning for efficient queries - O(log n + k) query
 * 
 * Algorithms Used:
 * - QuickSort: Sorting enemies by priority
 * - Binary Search: Finding enemies in sorted arrays
 * - A*: Pathfinding (referenced in BTT tasks)
 */
UCLASS()
class PROJECT_GOLDFISH_API AEnemyDirectorEnhanced : public AActor
{
    GENERATED_BODY()
    
public:    
    AEnemyDirectorEnhanced();

    // Event delegates
    UPROPERTY(BlueprintAssignable)
    FOnWaveChanged OnWaveChanged;

    virtual void Tick(float DeltaTime) override;

    // Wave management
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Waves")
    int IInitialWaveSpawnCount = 5;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Waves")
    int ICurrentWave = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Waves")
    int IFinalGrowthWave = 50;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Waves")
    int IMaxEnemiesInWave = 666;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Waves")
    int IMaxEnemyArenaCapacity = 50;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Waves")
    int IWaveMaxEnemyArenaCapacityReached = 22;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Waves")
    int IMaxEnemiesInArena = 5;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Waves")
    float FSecondsBeforeWaveStarts = 4.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Waves")
    float FSecondsBeforeWaveEnds = 4.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Management")
    TArray<FVector> PSpawnLocations;

    int ICurrentWaveSize = 0;
    int IWaveKills = 0;
    TArray<AActor*> PEnemies;

    // Enhanced functions using custom data structures
    UFUNCTION(BlueprintCallable, Category="Enemy Management")
    void AttemptSpawnEnemies();

    UFUNCTION(BlueprintCallable, Category="Enemy Management")
    AActor* FindNearestEnemy(const FVector& Position, float MaxDistance = -1.0f);

    UFUNCTION(BlueprintCallable, Category="Enemy Management")
    TArray<AActor*> FindEnemiesInRadius(const FVector& Center, float Radius);

    UFUNCTION(BlueprintCallable, Category="Enemy Management")
    TArray<FEnemyPriority> GetSortedEnemiesByThreat();

    UFUNCTION(BlueprintCallable, Category="Enemy Management")
    AActor* FindEnemyByID(int32 EnemyID);

    UFUNCTION()
    void ConfirmEnemyKilled();

    UFUNCTION(BlueprintCallable, Category="HUD")
    void RefreshUI();

    // Performance metrics
    UFUNCTION(BlueprintPure, Category="Performance")
    void GetPerformanceMetrics(float& OutQuadtreeQueryTime, float& OutSortTime, 
                               float& OutSearchTime, int32& OutTotalQueries) const;

protected:
    virtual void BeginPlay() override;

    void EndWaveDelayedCallback();
    void NextWaveDelayedCallback();

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Speed")
    float m_fGlobalFinalMaxWalkSpeed = 400.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Speed")
    float m_fGlobalFinalMinWalkSpeed = 200.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Speed")
    float m_fGlobalMaxWalkSpeed = 120.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Speed")
    float m_fGlobalMinWalkSpeed = 70.0f;

    FTimerHandle pTimerHandleCurrent;

private:
    // Custom Data Structures
    CustomHashMap<int32, AActor*> EnemyRegistry; // O(1) enemy lookup by ID
    CustomPriorityQueue<FEnemyPriority> ThreatQueue; // O(log n) priority management
    TSharedPtr<FQuadtree> SpatialPartition; // O(log n + k) spatial queries

    // Performance tracking
    float QuadtreeQueryTime;
    float SortTime;
    float SearchTime;
    int32 TotalQueries;

    bool m_bWaveIntermission;
    int32 NextEnemyID;

    // Helper functions
    void ClearCurrentTimer();
    void UpdateSpatialPartition();
    void RebuildEnemyRegistry();
    void UpdateEnemyPriorities(const FVector& PlayerLocation);
    void ModifyWaveSpeeds();
    int UpdateWaveSize();
    int UpdateEnemyArenaCapacity();
    void UpdateWaveParameters();
    void NextWave();
    void EndWave();
    TArray<AActor*> GetAllEnemiesInArena();
    TArray<AActor*> GetAllEnemiesInPool();
};
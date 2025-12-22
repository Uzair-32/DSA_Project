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

// Multicast delegate to broadcast wave changes to UI or other listeners.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWaveChanged, int, iWave);

/**
 * Enhanced Enemy Priority structure.
 * Used for sorting enemies based on threat levels (distance, ID).
 * Compatible with CustomPriorityQueue and SortingAlgorithms.
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

    // Default Constructor
    FEnemyPriority()
        : EnemyID(-1)
        , Priority(0.0f)
        , DistanceToPlayer(0.0f)
    {
    }

    // Parameterized Constructor
    FEnemyPriority(int32 InID, float InPriority, float InDistance)
        : EnemyID(InID)
        , Priority(InPriority)
        , DistanceToPlayer(InDistance)
    {
    }

    // Operator overloading for sorting algorithms (QuickSort).
    bool operator<(const FEnemyPriority& Other) const
    {
        return Priority < Other.Priority;
    }

    // Operator overloading for equality checks.
    bool operator==(const FEnemyPriority& Other) const
    {
        return EnemyID == Other.EnemyID;
    }
};

/**
 * AEnemyDirectorEnhanced:
 * Enhanced enemy director utilizing custom C++ data structures and algorithms
 * to optimize enemy management, spatial queries, and prioritization.
 * * Optimization Overview:
 * - CustomHashMap: Provides O(1) access to enemy instances via ID.
 * - CustomPriorityQueue: Manages enemy threat levels with O(log n) efficiency.
 * - Quadtree: Spatial partitioning allows for O(log n) area searches instead of O(n) iteration.
 * * Algorithms:
 * - QuickSort: Used for ranking enemies by threat.
 * - Binary Search: (Available via SearchAlgorithms header).
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

    // Called every frame to update spatial partitions and game logic.
    virtual void Tick(float DeltaTime) override;

    // --- Wave Configuration Properties ---

    // Number of enemies in the first wave.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Waves")
    int IInitialWaveSpawnCount = 5;

    // Current wave index.
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Waves")
    int ICurrentWave = 0;

    // The wave number where difficulty scaling stops.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Waves")
    int IFinalGrowthWave = 50;

    // Hard cap for total enemies in a single wave.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Waves")
    int IMaxEnemiesInWave = 666;

    // Absolute maximum enemies allowed in the arena (performance cap).
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Waves")
    int IMaxEnemyArenaCapacity = 50;

    // Wave number where the arena capacity hits its max.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Waves")
    int IWaveMaxEnemyArenaCapacityReached = 22;

    // Current max active enemies allowed (scales with wave).
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Waves")
    int IMaxEnemiesInArena = 5;

    // Time delay before wave begins.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Waves")
    float FSecondsBeforeWaveStarts = 4.0f;

    // Time delay after wave ends.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Waves")
    float FSecondsBeforeWaveEnds = 4.0f;

    // Locations where enemies can spawn.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Management")
    TArray<FVector> PSpawnLocations;

    // Internal counters
    int ICurrentWaveSize = 0;
    int IWaveKills = 0;
    
    // Master list of all managed enemy actors.
    TArray<AActor*> PEnemies;

    // --- Enhanced Management Functions (Custom DS Integration) ---

    // Spawns enemies from the pool into the arena.
    UFUNCTION(BlueprintCallable, Category="Enemy Management")
    void AttemptSpawnEnemies();

    // Finds the closest enemy using Quadtree spatial partitioning (O(log n)).
    UFUNCTION(BlueprintCallable, Category="Enemy Management")
    AActor* FindNearestEnemy(const FVector& Position, float MaxDistance = -1.0f);

    // Finds all enemies in a specific radius using Quadtree (O(log n + k)).
    UFUNCTION(BlueprintCallable, Category="Enemy Management")
    TArray<AActor*> FindEnemiesInRadius(const FVector& Center, float Radius);

    // Returns a list of enemies sorted by threat level using QuickSort.
    UFUNCTION(BlueprintCallable, Category="Enemy Management")
    TArray<FEnemyPriority> GetSortedEnemiesByThreat();

    // Retrieves an enemy by their unique ID using HashMap (O(1)).
    UFUNCTION(BlueprintCallable, Category="Enemy Management")
    AActor* FindEnemyByID(int32 EnemyID);

    // Callback when an enemy dies.
    UFUNCTION()
    void ConfirmEnemyKilled();

    // Updates HUD elements.
    UFUNCTION(BlueprintCallable, Category="HUD")
    void RefreshUI();

    // Returns performance stats for the custom data structures.
    UFUNCTION(BlueprintPure, Category="Performance")
    void GetPerformanceMetrics(float& OutQuadtreeQueryTime, float& OutSortTime, 
                               float& OutSearchTime, int32& OutTotalQueries) const;

protected:
    virtual void BeginPlay() override;

    // Timer callbacks for wave flow.
    void EndWaveDelayedCallback();
    void NextWaveDelayedCallback();

    // Speed scaling parameters.
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
    // --- Custom Data Structures ---
    
    // Fast O(1) lookup map for enemy references.
    CustomHashMap<int32, AActor*> EnemyRegistry; 
    
    // Min-heap or Max-heap for priority management O(log n).
    CustomPriorityQueue<FEnemyPriority> ThreatQueue; 
    
    // Quadtree for optimized spatial queries O(log n).
    TSharedPtr<FQuadtree> SpatialPartition; 

    // --- Performance Tracking ---
    float QuadtreeQueryTime;
    float SortTime;
    float SearchTime;
    int32 TotalQueries;

    // Wave state flags.
    bool m_bWaveIntermission;
    int32 NextEnemyID;

    // --- Helper Functions ---
    void ClearCurrentTimer();
    
    // Rebuilds the Quadtree based on current enemy positions.
    void UpdateSpatialPartition();
    
    // Populates the HashMap.
    void RebuildEnemyRegistry();
    
    // Recalculates threat levels and updates the Priority Queue.
    void UpdateEnemyPriorities(const FVector& PlayerLocation);
    
    void ModifyWaveSpeeds();
    int UpdateWaveSize();
    int UpdateEnemyArenaCapacity();
    void UpdateWaveParameters();
    void NextWave();
    void EndWave();
    
    // Object pooling helpers.
    TArray<AActor*> GetAllEnemiesInArena();
    TArray<AActor*> GetAllEnemiesInPool();
};
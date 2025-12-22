// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyDirectorEnhanced.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Enemy.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "FpsCharacter.h"

AEnemyDirectorEnhanced::AEnemyDirectorEnhanced()
{
    PrimaryActorTick.bCanEverTick = true;
    
    m_bWaveIntermission = false;
    NextEnemyID = 0;
    QuadtreeQueryTime = 0.0f;
    SortTime = 0.0f;
    SearchTime = 0.0f;
    TotalQueries = 0;

    // Initialize HashMap with capacity for expected enemies to minimize collisions/resizing.
    EnemyRegistry = CustomHashMap<int32, AActor*>(128);
}

void AEnemyDirectorEnhanced::BeginPlay()
{
    Super::BeginPlay();
    
    // Get all instances of enemy actors in the level.
    auto world = GetWorld();
    UGameplayStatics::GetAllActorsOfClass(world, AEnemy::StaticClass(), PEnemies);

    // Build enemy registry with unique IDs - O(n) operation.
    RebuildEnemyRegistry();

    // Initialize spatial partition (Quadtree).
    // Assuming a 10000x10000 unit arena centered at origin.
    // This allows for logarithmic search complexity later.
    FVector2D ArenaCenter(0.0f, 0.0f);
    FVector2D ArenaHalfSize(5000.0f, 5000.0f);
    SpatialPartition = MakeShared<FQuadtree>(FQuadtreeBounds(ArenaCenter, ArenaHalfSize));

    NextWave();
}

void AEnemyDirectorEnhanced::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    if (!m_bWaveIntermission)
    {
        // Update spatial partition every frame for efficient queries.
        // Enemies move, so the tree structure must be refreshed.
        UpdateSpatialPartition();
        
        // Handle spawning logic.
        AttemptSpawnEnemies();
    }
}

void AEnemyDirectorEnhanced::RebuildEnemyRegistry()
{
    /*
     * Algorithm: HashMap Construction
     * Time Complexity: O(n) where n = number of enemies
     * Space Complexity: O(n)
     * * Purpose: Build fast O(1) lookup table for enemies by ID
     */
    
    EnemyRegistry.Clear();
    NextEnemyID = 0;

    for (AActor* Actor : PEnemies)
    {
        // Insert enemy into custom hash map.
        EnemyRegistry.Insert(NextEnemyID, Actor);
        NextEnemyID++;
    }

    UE_LOG(LogTemp, Log, TEXT("[Data Structure] Enemy Registry built: %d enemies, Load Factor: %.2f"),
        EnemyRegistry.GetSize(), EnemyRegistry.GetLoadFactor());
}

void AEnemyDirectorEnhanced::UpdateSpatialPartition()
{
    /*
     * Algorithm: Quadtree Reconstruction
     * Time Complexity: O(n log n) where n = number of enemies
     * Space Complexity: O(n)
     * * Purpose: Rebuild spatial partition for efficient range queries
     */
    
    double StartTime = FPlatformTime::Seconds();

    // Clear previous frame's data.
    SpatialPartition->Clear();

    // Re-insert all active arena enemies into the Quadtree.
    for (AActor* Actor : PEnemies)
    {
        AEnemy* Enemy = Cast<AEnemy>(Actor);
        if (Enemy && Enemy->BInArena)
        {
            FVector Location = Enemy->GetActorLocation();
            FVector2D Location2D(Location.X, Location.Y);
            FQuadtreePoint Point(Location2D, Actor);
            SpatialPartition->Insert(Point);
        }
    }

    double EndTime = FPlatformTime::Seconds();
    QuadtreeQueryTime = static_cast<float>(EndTime - StartTime);

    // Debug log every 60 frames (~1 sec) to monitor overhead.
    if (GetWorld()->GetTimeSeconds() > 1.0f && FMath::Fmod(GetWorld()->GetTimeSeconds(), 1.0f) < 0.016f)
    {
        UE_LOG(LogTemp, Log, TEXT("[Quadtree] Updated spatial partition: %d enemies, Time: %.4f ms"),
            SpatialPartition->GetSize(), QuadtreeQueryTime * 1000.0f);
    }
}

void AEnemyDirectorEnhanced::UpdateEnemyPriorities(const FVector& PlayerLocation)
{
    /*
     * Algorithm: Priority Queue Construction
     * Time Complexity: O(n log n) where n = number of enemies
     * Space Complexity: O(n)
     * * Purpose: Build priority queue of threats for AI decision making
     */
    
    ThreatQueue.Clear();

    for (AActor* Actor : PEnemies)
    {
        AEnemy* Enemy = Cast<AEnemy>(Actor);
        if (Enemy && Enemy->BInArena)
        {
            float Distance = FVector::Dist(Enemy->GetActorLocation(), PlayerLocation);
            
            // Calculate threat priority (closer = higher threat = lower priority value for min-heap)
            float Threat = Distance / 100.0f; // Normalize
            
            // Reverse lookup ID from Map (Ideally, the Enemy actor would store its own ID to avoid this loop).
            int32 EnemyID = -1;
            TArray<int32> Keys = EnemyRegistry.GetKeys();
            for (int32 Key : Keys)
            {
                AActor* RegisteredActor;
                if (EnemyRegistry.Find(Key, RegisteredActor) && RegisteredActor == Actor)
                {
                    EnemyID = Key;
                    break;
                }
            }

            // Enqueue into Custom Priority Queue.
            if (EnemyID >= 0)
            {
                FEnemyPriority Priority(EnemyID, Threat, Distance);
                ThreatQueue.Enqueue(Priority, Threat);
            }
        }
    }

    UE_LOG(LogTemp, Log, TEXT("[Priority Queue] Updated threat queue: %d enemies prioritized"),
        ThreatQueue.Size());
}

AActor* AEnemyDirectorEnhanced::FindNearestEnemy(const FVector& Position, float MaxDistance)
{
    /*
     * Algorithm: Quadtree Nearest Neighbor Search
     * Time Complexity: O(log n) average
     * Space Complexity: O(1)
     * * Purpose: Find closest enemy using spatial partitioning
     */
    
    double StartTime = FPlatformTime::Seconds();

    FVector2D Position2D(Position.X, Position.Y);
    FQuadtreePoint NearestPoint;
    
    // Execute search on custom data structure.
    bool bFound = SpatialPartition->FindNearest(Position2D, NearestPoint, MaxDistance);

    double EndTime = FPlatformTime::Seconds();
    SearchTime = static_cast<float>(EndTime - StartTime);
    TotalQueries++;

    if (bFound)
    {
        UE_LOG(LogTemp, Verbose, TEXT("[Quadtree Search] Found nearest enemy in %.4f ms"),
            SearchTime * 1000.0f);
        return NearestPoint.Data;
    }

    return nullptr;
}

TArray<AActor*> AEnemyDirectorEnhanced::FindEnemiesInRadius(const FVector& Center, float Radius)
{
    /*
     * Algorithm: Quadtree Range Query
     * Time Complexity: O(log n + k) where k = number of results
     * Space Complexity: O(k)
     * * Purpose: Find all enemies within radius using spatial partitioning
     */
    
    double StartTime = FPlatformTime::Seconds();

    TArray<AActor*> Result;
    TArray<FQuadtreePoint> Points;
    
    // Query the custom Quadtree structure.
    FVector2D Center2D(Center.X, Center.Y);
    SpatialPartition->QueryRadius(Center2D, Radius, Points);

    // Convert Quadtree points back to Actor pointers.
    for (const FQuadtreePoint& Point : Points)
    {
        if (Point.Data)
        {
            Result.Add(Point.Data);
        }
    }

    double EndTime = FPlatformTime::Seconds();
    SearchTime = static_cast<float>(EndTime - StartTime);
    TotalQueries++;

    UE_LOG(LogTemp, Verbose, TEXT("[Quadtree Query] Found %d enemies in radius in %.4f ms"),
        Result.Num(), SearchTime * 1000.0f);

    return Result;
}

TArray<FEnemyPriority> AEnemyDirectorEnhanced::GetSortedEnemiesByThreat()
{
    /*
     * Algorithm: QuickSort
     * Time Complexity: O(n log n) average, O(n²) worst case
     * Space Complexity: O(log n) for recursion stack
     * * Purpose: Sort enemies by threat level for display/targeting
     */
    
    double StartTime = FPlatformTime::Seconds();

    // Build array of enemy priorities.
    TArray<FEnemyPriority> Priorities;
    
    // Get player location for distance calculation.
    AFpsCharacter* Player = Cast<AFpsCharacter>(
        GetWorld()->GetFirstPlayerController()->GetCharacter()
    );
    FVector PlayerLocation = Player ? Player->GetActorLocation() : FVector::ZeroVector;

    // Populate unordered list.
    for (AActor* Actor : PEnemies)
    {
        AEnemy* Enemy = Cast<AEnemy>(Actor);
        if (Enemy && Enemy->BInArena)
        {
            float Distance = FVector::Dist(Enemy->GetActorLocation(), PlayerLocation);
            
            // Find enemy ID (Reverse lookup).
            int32 EnemyID = -1;
            TArray<int32> Keys = EnemyRegistry.GetKeys();
            for (int32 Key : Keys)
            {
                AActor* RegisteredActor;
                if (EnemyRegistry.Find(Key, RegisteredActor) && RegisteredActor == Actor)
                {
                    EnemyID = Key;
                    break;
                }
            }

            if (EnemyID >= 0)
            {
                float Threat = 10000.0f / (Distance + 1.0f); // Higher threat for closer enemies.
                Priorities.Add(FEnemyPriority(EnemyID, Threat, Distance));
            }
        }
    }

    // Sort using custom QuickSort implementation - O(n log n).
    SortingAlgorithms::QuickSort(Priorities, 
        [](const FEnemyPriority& A, const FEnemyPriority& B)
        {
            return A.Priority > B.Priority; // Descending order (highest threat first).
        }
    );

    double EndTime = FPlatformTime::Seconds();
    SortTime = static_cast<float>(EndTime - StartTime);

    UE_LOG(LogTemp, Log, TEXT("[QuickSort] Sorted %d enemies by threat in %.4f ms"),
        Priorities.Num(), SortTime * 1000.0f);

    return Priorities;
}

AActor* AEnemyDirectorEnhanced::FindEnemyByID(int32 EnemyID)
{
    /*
     * Algorithm: HashMap Lookup
     * Time Complexity: O(1) average, O(n) worst case
     * Space Complexity: O(1)
     * * Purpose: Fast enemy lookup by unique ID
     */
    
    double StartTime = FPlatformTime::Seconds();

    AActor* FoundEnemy;
    // Perform O(1) lookup.
    bool bFound = EnemyRegistry.Find(EnemyID, FoundEnemy);

    double EndTime = FPlatformTime::Seconds();
    SearchTime = static_cast<float>(EndTime - StartTime);

    if (bFound)
    {
        UE_LOG(LogTemp, Verbose, TEXT("[HashMap] Found enemy ID %d in %.4f µs"),
            EnemyID, SearchTime * 1000000.0f);
        return FoundEnemy;
    }

    return nullptr;
}

void AEnemyDirectorEnhanced::GetPerformanceMetrics(float& OutQuadtreeQueryTime, float& OutSortTime,
                                                   float& OutSearchTime, int32& OutTotalQueries) const
{
    // Return tracking data for debug HUD/Profiling.
    OutQuadtreeQueryTime = QuadtreeQueryTime;
    OutSortTime = SortTime;
    OutSearchTime = SearchTime;
    OutTotalQueries = TotalQueries;
}

// --- Original EnemyDirector Logic Implementation ---

void AEnemyDirectorEnhanced::AttemptSpawnEnemies()
{
    // Object Pooling: Retrieve lists.
    TArray<AActor*> pPooledEnemies = GetAllEnemiesInPool();
    TArray<AActor*> pArenaEnemies = GetAllEnemiesInArena();
    int iPooledEnemiesCount = pPooledEnemies.Num();
    int iArenaEnemiesCount = pArenaEnemies.Num();
    
    if (iPooledEnemiesCount == 0)
        return; // No enemies available in pool.

    // Calculate how many more enemies are needed.
    int iLeftToKill = ICurrentWaveSize - IWaveKills;
    int iLeftToSpawn = iLeftToKill - iArenaEnemiesCount;
    
    if (iLeftToSpawn <= 0)
        return;
    
    // Check arena capacity limits.
    int iArenaCapacityLeft = IMaxEnemiesInArena - iArenaEnemiesCount;
    if (iArenaCapacityLeft <= 0)
        return;

    // Determine actual spawn count for this tick.
    int iAmountSpawnable = UKismetMathLibrary::Min(iArenaCapacityLeft, iLeftToSpawn);
    iAmountSpawnable = UKismetMathLibrary::Min(iPooledEnemiesCount, iAmountSpawnable);
    
    // Activate enemies.
    for (int i = 0; i < iAmountSpawnable; ++i)
    {
        AEnemy* pEnemy = Cast<AEnemy>(pPooledEnemies[i]);

        // Re-bind death delegate.
        pEnemy->OnEnemyKilled.Clear();
        pEnemy->OnEnemyKilled.BindUFunction(this, "ConfirmEnemyKilled");

        // Teleport to random spawn point and enable.
        pEnemy->SetActorEnableCollision(false);
        auto spawnLocation = PSpawnLocations[FMath::RandRange(0, PSpawnLocations.Num() - 1)];
        auto enemyRotation = pEnemy->GetActorRotation();
        pEnemy->TeleportTo(spawnLocation, enemyRotation);
        pEnemy->BInArena = true;
        pEnemy->SetActorEnableCollision(true);
    }
}

int AEnemyDirectorEnhanced::UpdateWaveSize()
{
    // Linear growth calculation for total wave size.
    int iTotalEnemyGrowth = IMaxEnemiesInWave - IInitialWaveSpawnCount;
    float fRateOfGrowth = (float)iTotalEnemyGrowth / IFinalGrowthWave;
    int iNewEnemyCount = (ICurrentWave - 1) * fRateOfGrowth;
    ICurrentWaveSize = UKismetMathLibrary::Min(IMaxEnemiesInWave, (IInitialWaveSpawnCount + iNewEnemyCount));
    return ICurrentWaveSize;
}

int AEnemyDirectorEnhanced::UpdateEnemyArenaCapacity()
{
    // Linear growth calculation for concurrent enemy limit.
    int iTotalCapacityGrowth = IMaxEnemyArenaCapacity - IMaxEnemiesInArena;
    float fRateOfGrowth = (float)iTotalCapacityGrowth / IWaveMaxEnemyArenaCapacityReached;
    int iNewCapacity = (ICurrentWave - 1) * fRateOfGrowth;
    IMaxEnemiesInArena = UKismetMathLibrary::Min(IMaxEnemyArenaCapacity, (IMaxEnemiesInArena + iNewCapacity));
    return IMaxEnemiesInArena;
}

void AEnemyDirectorEnhanced::UpdateWaveParameters()
{
    // Increment wave and reset kills.
    ICurrentWave++;
    IWaveKills = 0;
    UpdateWaveSize();
    UpdateEnemyArenaCapacity();

    // Increase difficulty (Speed).
    m_fGlobalMaxWalkSpeed = FMath::Clamp(m_fGlobalMaxWalkSpeed + 50.0f, 0, m_fGlobalFinalMaxWalkSpeed);
    m_fGlobalMinWalkSpeed = FMath::Clamp(m_fGlobalMinWalkSpeed + 15.0f, 0, m_fGlobalFinalMinWalkSpeed);
}

void AEnemyDirectorEnhanced::NextWave()
{
    // Setup parameters for the new wave.
    UpdateWaveParameters();
    OnWaveChanged.Broadcast(ICurrentWave);
    
    // Delay start.
    UWorld* pWorld = GetWorld();
    ClearCurrentTimer();
    pWorld->GetTimerManager().SetTimer(pTimerHandleCurrent, this, 
        &AEnemyDirectorEnhanced::NextWaveDelayedCallback, FSecondsBeforeWaveStarts, false);
}

void AEnemyDirectorEnhanced::EndWave()
{
    // Start intermission.
    m_bWaveIntermission = true;
    UWorld* pWorld = GetWorld();
    ClearCurrentTimer();
    pWorld->GetTimerManager().SetTimer(pTimerHandleCurrent, this,
        &AEnemyDirectorEnhanced::EndWaveDelayedCallback, FSecondsBeforeWaveEnds, false);
}

TArray<AActor*> AEnemyDirectorEnhanced::GetAllEnemiesInArena()
{
    TArray<AActor*> pEnemiesInArena = TArray<AActor*>();
    for (AActor* actor : PEnemies)
    {
        AEnemy* pEnemy = Cast<AEnemy>(actor);
        if (pEnemy->BInArena)
        {
            pEnemiesInArena.Add(actor);
        }
    }
    return pEnemiesInArena;
}

TArray<AActor*> AEnemyDirectorEnhanced::GetAllEnemiesInPool()
{
    TArray<AActor*> pEnemiesInPool = TArray<AActor*>();
    for (AActor* actor : PEnemies)
    {
        AEnemy* pEnemy = Cast<AEnemy>(actor);
        if (!pEnemy->BInArena)
        {
            pEnemiesInPool.Add(actor);
        }
    }
    return pEnemiesInPool;
}

void AEnemyDirectorEnhanced::ModifyWaveSpeeds()
{
    // Apply speed settings to all enemies based on wave difficulty.
    for (int i = 0; i < PEnemies.Num(); ++i)
    {
        AEnemy* pEnemy = Cast<AEnemy>(PEnemies[i]);
        float maxWalkSpeed = FMath::RandRange(m_fGlobalMinWalkSpeed, m_fGlobalMaxWalkSpeed);
        float finalSpeed = maxWalkSpeed + pEnemy->GetBaseSpeed();
        pEnemy->GetCharacterMovement()->MaxWalkSpeed = finalSpeed;
    }
}

void AEnemyDirectorEnhanced::ConfirmEnemyKilled()
{
    IWaveKills++;

    // Check if wave is cleared.
    if (IWaveKills >= ICurrentWaveSize)
    {
        EndWave();
    }
}

void AEnemyDirectorEnhanced::RefreshUI()
{
    OnWaveChanged.Broadcast(ICurrentWave);
}

void AEnemyDirectorEnhanced::EndWaveDelayedCallback()
{
    NextWave();
}

void AEnemyDirectorEnhanced::NextWaveDelayedCallback()
{
    // Apply speeds and begin spawning.
    ModifyWaveSpeeds();
    m_bWaveIntermission = false;
}

void AEnemyDirectorEnhanced::ClearCurrentTimer()
{
    UWorld* pWorld = GetWorld();
    if (pWorld == nullptr)
        return;
    
    pWorld->GetTimerManager().ClearTimer(pTimerHandleCurrent);
}
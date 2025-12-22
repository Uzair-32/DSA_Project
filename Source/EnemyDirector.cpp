
#include "EnemyDirector.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Enemy.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "FpsCharacter.h"

// Sets default values
AEnemyDirector::AEnemyDirector()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AEnemyDirector::AttemptSpawnEnemies()
{
	// Get lists of active and inactive enemies.
	TArray<AActor*> pPooledEnemies = GetAllEnemiesInPool();
	TArray<AActor*> pArenaEnemies = GetAllEnemiesInArena();
	int iPooledEnemiesCount = pPooledEnemies.Num();
	int iArenaEnemiesCount = pArenaEnemies.Num();

	if (iPooledEnemiesCount == 0)
		return; // No enemies available to take from the pool (limit reached).

	// Calculate how many enemies are left to spawn in this specific wave.
	int iLeftToKill = ICurrentWaveSize - IWaveKills;
	int iLeftToSpawn = iLeftToKill - iArenaEnemiesCount;
	
	if (iLeftToSpawn <= 0)
		return; // Wave spawning complete.
	
	// Limit the amount of enemies we can spawn at once to our chosen limit (IMaxEnemiesInArena).
	int iArenaCapacityLeft = IMaxEnemiesInArena - iArenaEnemiesCount;
	if (iArenaCapacityLeft <= 0)
		return; // Arena is currently full.

	// Determine exactly how many to spawn this tick (min of capacity left, enemies left in wave, or pool size).
	int iAmountSpawnable = UKismetMathLibrary::Min(iArenaCapacityLeft, iLeftToSpawn);
	iAmountSpawnable = UKismetMathLibrary::Min(iPooledEnemiesCount, iAmountSpawnable);

	for (int i = 0; i < iAmountSpawnable; ++i)
	{
		AEnemy* pEnemy = Cast<AEnemy>(pPooledEnemies[i]);

		// Bind delegates to track when this specific enemy dies.
		pEnemy->OnEnemyKilled.Clear();
		pEnemy->OnEnemyKilled.BindUFunction(this, "ConfirmEnemyKilled");

		// Disable collisions temporarily to prevent physics issues during teleport.
		pEnemy->SetActorEnableCollision(false);
		
		// Pick a random spawn point.
		auto spawnLocation = PSpawnLocations[FMath::RandRange(0, PSpawnLocations.Num() - 1)];
		auto enemyRotation = pEnemy->GetActorRotation();
		
		// Move enemy to arena and activate.
		pEnemy->TeleportTo(spawnLocation, enemyRotation);
		pEnemy->BInArena = true;
		pEnemy->SetActorEnableCollision(true);
	}
}

int AEnemyDirector::UpdateWaveSize()
{
	// Calculate total enemies for the current wave using linear growth.
	int iTotalEnemyGrowth = IMaxEnemiesInWave - IInitialWaveSpawnCount;
	float fRateOfGrowth = (float)iTotalEnemyGrowth / IFinalGrowthWave;
	int iNewEnemyCount = (ICurrentWave - 1) * fRateOfGrowth;
	
	// Ensure we don't exceed the hard cap.
	ICurrentWaveSize = UKismetMathLibrary::Min(IMaxEnemiesInWave, (IInitialWaveSpawnCount + iNewEnemyCount)); 
    return ICurrentWaveSize;
}

int AEnemyDirector::UpdateEnemyArenaCapacity()
{
	// Calculate the max concurrent enemies allowed in the arena for this wave.
    int iTotalCapacityGrowth = IMaxEnemyArenaCapacity - IMaxEnemiesInArena;
	float fRateOfGrowth = (float)iTotalCapacityGrowth / IWaveMaxEnemyArenaCapacityReached;
	int iNewCapacity = (ICurrentWave - 1) * fRateOfGrowth;
	
	// Ensure we don't exceed the hard cap.
	IMaxEnemiesInArena = UKismetMathLibrary::Min(IMaxEnemyArenaCapacity, (IMaxEnemiesInArena + iNewCapacity));
	return IMaxEnemiesInArena;
}

void AEnemyDirector::UpdateWaveParameters()
{
	// Reset and move to the next wave index.
	ICurrentWave++;
	IWaveKills = 0;
	
	// Recalculate difficulty settings.
	UpdateWaveSize();
	UpdateEnemyArenaCapacity();

	// Increase enemy speed difficulty, clamped to maximums.
	m_fGlobalMaxWalkSpeed = FMath::Clamp(m_fGlobalMaxWalkSpeed + 50.0f, 0, m_fGlobalFinalMaxWalkSpeed);
	m_fGlobalMinWalkSpeed = FMath::Clamp(m_fGlobalMinWalkSpeed + 15.0f, 0, m_fGlobalFinalMinWalkSpeed);
}

void AEnemyDirector::NextWave()
{
	UpdateWaveParameters();

	// Notify listeners (UI) that the wave has changed.
	OnWaveChanged.Broadcast(ICurrentWave);
	
	// Delay spawning of enemies to give the player a breather.
	UWorld* pWorld = GetWorld();

	ClearCurrentTimer();
	pWorld->GetTimerManager().SetTimer(pTimerHandleCurrent, this, &AEnemyDirector::NextWaveDelayedCallback, FSecondsBeforeWaveStarts, false);
}

void AEnemyDirector::EndWave()
{
	// Start the intermission state.
	m_bWaveIntermission = true;
	UWorld* pWorld = GetWorld();

	// Set timer to start the next wave sequence.
	ClearCurrentTimer();
	pWorld->GetTimerManager().SetTimer(pTimerHandleCurrent, this, &AEnemyDirector::EndWaveDelayedCallback, FSecondsBeforeWaveEnds, false);
}

void AEnemyDirector::SpawnMoreEnemies()
{
	// Check if there is room and need to spawn more enemies immediately.
	int iEnemiesInArenaCount = GetAllEnemiesInArena().Num();
	int iLeftToSpawn = (ICurrentWaveSize - IWaveKills) - iEnemiesInArenaCount;
	bool bArenaFull = iEnemiesInArenaCount == IMaxEnemiesInArena;
	
	if ((iLeftToSpawn > 0) && !bArenaFull)
	{
		AttemptSpawnEnemies();
	}
}

TArray<AActor*> AEnemyDirector::GetAllEnemiesInArena()
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

TArray<AActor*> AEnemyDirector::GetAllEnemiesInPool()
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

void AEnemyDirector::ModifyWaveSpeeds()
{
	// Iterate through all enemies and randomize their speeds based on current wave difficulty.
	for (int i = 0; i < PEnemies.Num(); ++i)
	{
		AEnemy* pEnemy = Cast<AEnemy>(PEnemies[i]);
		float maxWalkSpeed = FMath::RandRange(m_fGlobalMinWalkSpeed, m_fGlobalMaxWalkSpeed);
		
		// Add the specific enemy's base speed to the global modifier.
		float finalSpeed = maxWalkSpeed + pEnemy->GetBaseSpeed();
		pEnemy->GetCharacterMovement()->MaxWalkSpeed = finalSpeed;
	}
}

void AEnemyDirector::ConfirmEnemyKilled()
{
	IWaveKills++;

	// Check if the wave is complete.
	if (IWaveKills >= ICurrentWaveSize)
	{
		EndWave();
	}
}

void AEnemyDirector::RefreshUI()
{
	OnWaveChanged.Broadcast(ICurrentWave);
}

// Called when the game starts or when spawned
void AEnemyDirector::BeginPlay()
{
	Super::BeginPlay();
	
	// Get all pre-placed instances of enemy actors and store in the master list.
	// This assumes enemies are placed in the level or pooled beforehand.
	auto world = GetWorld();
	UGameplayStatics::GetAllActorsOfClass(world, AEnemy::StaticClass(), PEnemies);

	// Start the first wave.
	NextWave();
}

void AEnemyDirector::EndWaveDelayedCallback()
{
	NextWave();
}

void AEnemyDirector::NextWaveDelayedCallback()
{
	// Apply new speeds and enable spawning.
	ModifyWaveSpeeds();
	m_bWaveIntermission = false;
}

void AEnemyDirector::ClearCurrentTimer()
{
	UWorld* pWorld = GetWorld();
	if (pWorld == nullptr)
		return;
	
	pWorld->GetTimerManager().ClearTimer(pTimerHandleCurrent);
}

// Called every frame
void AEnemyDirector::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// Continuously attempt to spawn enemies during active wave play.
	if (!m_bWaveIntermission)
	{
		AttemptSpawnEnemies();
	}
}
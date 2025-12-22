// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerStats.h"

/**
 * Constructor
 *
 * Initializes the PlayerStats actor and disables ticking since
 * point updates are event-driven and do not require per-frame logic.
 */
APlayerStats::APlayerStats()
{
    // Disable Tick for performance optimization
    PrimaryActorTick.bCanEverTick = false;

    // Initialize player points to zero at game start
    m_iPoints = 0;
}

/**
 * Retrieves the current player point total.
 */
int APlayerStats::GetPoints()
{
    return m_iPoints;
}

/**
 * Adds points to the player's total while enforcing defined bounds.
 * Broadcasts an update event after modification.
 */
int APlayerStats::AddPoints(int amount)
{
    // Safely update points within valid range
    m_iPoints = FMath::Clamp(m_iPoints + amount, 0, MaximumPoints);

    // Notify all listeners (e.g., UI, gameplay systems)
    OnPointsChanged.Broadcast(m_iPoints);

    return m_iPoints;
}

/**
 * Removes points from the player's total while enforcing defined bounds.
 * Broadcasts an update event after modification.
 */
int APlayerStats::RemovePoints(int amount)
{
    // Safely update points within valid range
    m_iPoints = FMath::Clamp(m_iPoints - amount, 0, MaximumPoints);

    // Notify all listeners (e.g., UI, gameplay systems)
    OnPointsChanged.Broadcast(m_iPoints);

    return m_iPoints;
}

/**
 * Called when the game starts or when the actor is spawned.
 * Broadcasts the initial point value so dependent systems
 * can initialize correctly.
 */
void APlayerStats::BeginPlay()
{
    Super::BeginPlay();

    // Ensure initial point value is communicated to listeners
    OnPointsChanged.Broadcast(m_iPoints);
}

/**
 * Called every frame.
 * Currently unused as ticking is disabled.
 */
void APlayerStats::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

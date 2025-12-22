#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlayerStats.generated.h"

/**
 * Delegate broadcast whenever the player's point total changes.
 *
 * @param iPoints The updated total number of points.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPointsChanged, int, iPoints);

/**
 * APlayerStats
 *
 * Actor responsible for managing player-related statistical data,
 * specifically the player's point system. This class exposes point
 * operations to Blueprints and notifies listeners when values change.
 */
UCLASS()
class PROJECT_GOLDFISH_API APlayerStats : public AActor
{
	GENERATED_BODY()

public:
	/**
	 * Default constructor.
	 * Initializes the PlayerStats actor.
	 */
	APlayerStats();

	/**
	 * Event triggered whenever the player's point total is updated.
	 * Can be bound to UI widgets or gameplay systems via Blueprints.
	 */
	UPROPERTY(BlueprintAssignable)
	FOnPointsChanged OnPointsChanged;

	/**
	 * Upper limit for player points to prevent overflow
	 * and maintain game balance.
	 */
	const int MaximumPoints = 999999;

	/**
	 * Retrieves the current total number of player points.
	 *
	 * @return Current point total.
	 */
	UFUNCTION(BlueprintCallable, Category = "Points")
	int GetPoints();

	/**
	 * Increases the player's points by the specified amount.
	 * The total is clamped to the defined maximum.
	 *
	 * @param amount Number of points to add.
	 * @return Updated point total.
	 */
	UFUNCTION(BlueprintCallable, Category = "Points")
	int AddPoints(int amount);

	/**
	 * Decreases the player's points by the specified amount.
	 * The total is clamped to zero.
	 *
	 * @param amount Number of points to remove.
	 * @return Updated point total.
	 */
	UFUNCTION(BlueprintCallable, Category = "Points")
	int RemovePoints(int amount);

protected:
	/**
	 * Called when the game starts or when the actor is spawned.
	 * Used for initialization logic.
	 */
	virtual void BeginPlay() override;

public:	
	/**
	 * Called every frame.
	 *
	 * @param DeltaTime Time elapsed since the previous frame.
	 */
	virtual void Tick(float DeltaTime) override;

private:
	/**
	 * Stores the current number of player points.
	 * Kept private to enforce controlled access via functions.
	 */
	int m_iPoints = 0;
};

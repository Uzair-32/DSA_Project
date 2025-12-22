// Fill out your copyright notice in the Description page of Project Settings.


#include "IsPlayerInRange.h"
#include "Kismet/GameplayStatics.h"
#include "Enemy_Controller.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EnemyKeys.h"
#include "Enemy.h"

/**
 * Constructor
 * Initializes the service to notify when it becomes relevant and sets a descriptive name.
 */
UIsPlayerInRange::UIsPlayerInRange()
{
    // Ensures OnBecomeRelevant is called when service activates
    bNotifyBecomeRelevant = true;
    // Display name in Behavior Tree editor
    NodeName = TEXT("Is Player In Range"); 
}
/**
 * OnBecomeRelevant:
 * Called when the service is activated in the Behavior Tree.
 * Updates the "IsPlayerInRange" blackboard key based on distance between enemy and player.
 *
 * @param behaviorTreeComponent - Reference to the Behavior Tree component running this service
 * @param nodeMemory - Pointer to memory used by this service (internal UE use)
 */
void UIsPlayerInRange::OnBecomeRelevant(UBehaviorTreeComponent& behaviorTreeComponent, uint8* nodeMemory)
{
    Super::OnBecomeRelevant(behaviorTreeComponent, nodeMemory);

    // Get AI controller & Pawn.
    AEnemy_Controller* pAiController = Cast<AEnemy_Controller>(behaviorTreeComponent.GetAIOwner());
    APawn* pEnemyPawn = pAiController->GetPawn();
    AEnemy* pEnemy = Cast<AEnemy>(pEnemyPawn);

    // Get Player Controller & Pawn.
    APlayerController* pPlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    AActor* pPlayerPawn = pPlayerController->GetPawn();

    // Calculate distance between enemy and player
    float fDistanceBetweenPawns = pEnemyPawn->GetDistanceTo(pPlayerPawn);
    // Check if the player is within attack range
    bool bWithinRange = fDistanceBetweenPawns <= pEnemy->GetAttackRange();
    // Update the AI blackboard key with the result
    pAiController->GetBlackboard()->SetValueAsBool(EnemyKeys::IsPlayerInRange, bWithinRange);
}

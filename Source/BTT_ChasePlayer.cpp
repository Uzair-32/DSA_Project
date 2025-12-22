
#include "BTT_ChasePlayer.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Runtime/NavigationSystem/Public/NavigationSystem.h"
#include "Enemy_Controller.h"
#include "EnemyKeys.h"
#include "Kismet/GameplayStatics.h"

// Constructor implementation
UBTT_ChasePlayer::UBTT_ChasePlayer(FObjectInitializer const& a_pObjectInit)
{
    // Set the name of the node as it will appear in the Behavior Tree editor graph
    NodeName = TEXT("Chase Player");
}

// Main execution function for the task
EBTNodeResult::Type UBTT_ChasePlayer::ExecuteTask(UBehaviorTreeComponent& pTreeComp, uint8* pNodeMem)
{
    // Retrieve the AI Controller owning this behavior tree
    AEnemy_Controller* const pAIController = Cast<AEnemy_Controller>(pTreeComp.GetAIOwner());
    
    // Get the Navigation System for the current world to ensure pathfinding is possible
    UNavigationSystemV1* pNavSystem = UNavigationSystemV1::GetCurrent(GetWorld());

    // Only proceed if the navigation system is valid (navmesh exists)
    if (pNavSystem != nullptr)
    {
        // 1. Retrieve the target destination vector from the Blackboard using the key defined in EnemyKeys.
        // 2. Instruct the AI Controller to move the pawn to that location using standard pathfinding.
        FVector pLocation  = pAIController->GetBlackboard()->GetValueAsVector(EnemyKeys::TargetLocation);
        pAIController->MoveToLocation(pLocation);
    }

    // Signal that the task has successfully initiated the action and can finish.
    FinishLatentTask(pTreeComp, EBTNodeResult::Succeeded);

    return EBTNodeResult::Succeeded;
}

#include "BTT_FindPlayerLocation.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Runtime/NavigationSystem/Public/NavigationSystem.h"
#include "Enemy_Controller.h"
#include "EnemyKeys.h"
#include "Kismet/GameplayStatics.h"

// Constructor implementation
UBTT_FindPlayerLocation::UBTT_FindPlayerLocation(FObjectInitializer const& a_pObjectInit)
{
    // Set the name of the node as it appears in the Behavior Tree GUI
    NodeName = TEXT("Find Player Location");
}

// Main execution function
EBTNodeResult::Type UBTT_FindPlayerLocation::ExecuteTask(UBehaviorTreeComponent& pTreeComp, uint8* pNodeMem)
{
    UWorld* pWorld = GetWorld();
    
    // Get the AI controller that owns this behavior tree
    AEnemy_Controller* const pAIController = Cast<AEnemy_Controller>(pTreeComp.GetAIOwner());
    
    // Get the navigation system to ensure valid navigation exists (though we rely mostly on player position here)
    UNavigationSystemV1* pNavSystem = UNavigationSystemV1::GetCurrent(pWorld);

    // Retrieve the player controller and the player pawn to find their location
    APlayerController* pPlayerController = UGameplayStatics::GetPlayerController(pWorld, 0);
    APawn* pPlayer = pPlayerController->GetPawn();

    if (pNavSystem != nullptr)
    {
        // Update the Blackboard key "TargetLocation" with the player's current world position.
        // This is the critical step that allows the "Chase" task to know where to go.
        pAIController->GetBlackboard()->SetValueAsVector(EnemyKeys::TargetLocation, pPlayer->GetActorLocation());
    }

    // Signal that the task finished successfully.
    FinishLatentTask(pTreeComp, EBTNodeResult::Succeeded);

    return EBTNodeResult::Succeeded;
}
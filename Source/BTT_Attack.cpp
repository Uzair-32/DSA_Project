

#include "BTT_Attack.h"
#include "Enemy_Controller.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EnemyKeys.h"

// Constructor implementation
UBTT_Attack::UBTT_Attack(FObjectInitializer const& pObjectInit)
{
    // Label the node in the editor.
    NodeName = TEXT("Attack");
}

// Main execution function
EBTNodeResult::Type UBTT_Attack::ExecuteTask(UBehaviorTreeComponent& pTreeComponent, uint8* pNodeMemory)
{
    // Retrieve the AI Controller and the specific Enemy Pawn being controlled.
    AEnemy_Controller* pAiController = Cast<AEnemy_Controller>(pTreeComponent.GetAIOwner());
    AEnemy* pEnemy = Cast<AEnemy>(pAiController->GetPawn());

    // Ensure the enemy isn't currently mid-swing to prevent animation clipping/spamming.
    if (AttackMontageFinished(pEnemy))
    {
        // Check the Blackboard state to see if the "IsPlayerInRange" service has determined valid range.
        bool bCanAttack = pAiController->GetBlackboard()->GetValueAsBool(EnemyKeys::IsPlayerInRange);
        
        // If animation is done and player is close enough, execute the attack.
        if (bCanAttack)
        {
            pEnemy->Attack();
        }
    }

    // Mark task as succeeded immediately (instant task).
    FinishLatentTask(pTreeComponent, EBTNodeResult::Succeeded);
    return EBTNodeResult::Succeeded;
}

// Helper function to check animation status
bool UBTT_Attack::AttackMontageFinished(AEnemy* pEnemy)
{
    UAnimInstance* pAnimInstance = pEnemy->GetMesh()->GetAnimInstance();
    
    // Safety check to ensure AnimInstance exists
    if (pAnimInstance != nullptr)
    {
        // Check if the specific attack montage assigned to this enemy has stopped playing.
        return pAnimInstance->Montage_GetIsStopped(pEnemy->GetAttackMontage());
    }
    
    return false;
}
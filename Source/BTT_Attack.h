
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "Enemy.h"
#include "BTT_Attack.generated.h"

/**
 * UBTT_Attack:
 * Custom Behavior Tree Task that handles the logic for triggering an enemy attack.
 * It ensures attacks only happen when the previous animation is finished and the target is in range.
 */
UCLASS()
class PROJECT_GOLDFISH_API UBTT_Attack : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	// Constructor: Sets the node name for the Behavior Tree Editor.
	UBTT_Attack(FObjectInitializer const& pObjectInit);

	// Main execution logic: Checks conditions (Animation state + Range) and triggers the attack.
	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& pTreeComponent, uint8* pNodeMemory);

	// Helper function to check if the specific attack animation montage has finished playing.
	bool AttackMontageFinished(AEnemy* pEnemy);
};
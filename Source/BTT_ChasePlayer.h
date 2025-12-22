#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTT_ChasePlayer.generated.h"

/**
 * UBTT_ChasePlayer:
 * Custom Behavior Tree Task for instructing the AI to chase a target.
 * This node commands the AI controller to move to the vector stored in the Blackboard.
 */
UCLASS()
class PROJECT_GOLDFISH_API UBTT_ChasePlayer : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	// Constructor: Sets up the node name for the Behavior Tree Editor.
	UBTT_ChasePlayer(FObjectInitializer const& a_pObjectInit);

	// The main execution logic called by the Behavior Tree when this node becomes active.
	// Returns Succeeded if the move request was sent successfully.
	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& pTreeComp, uint8* pNodeMem);
};
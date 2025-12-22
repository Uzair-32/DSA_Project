
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTT_FindPlayerLocation.generated.h"

/**
 * UBTT_FindPlayerLocation:
 * Custom Behavior Tree Task responsible for locating the player.
 * It calculates the player's position and stores it in the Blackboard for other tasks (like Chase) to use.
 */
UCLASS()
class PROJECT_GOLDFISH_API UBTT_FindPlayerLocation : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	// Constructor: Initializes the node name for the editor.
	UBTT_FindPlayerLocation(FObjectInitializer const& a_pObjectInit);

	// Main execution logic: Finds the player and updates the Blackboard key.
	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& pTreeComp, uint8* pNodeMem);
};
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "IsPlayerInRange.generated.h"

/**
 * UIsPlayerInRange:
 * Behavior for checkign if the player (B) is in range of Enemy (A).
 */
UCLASS()
class PROJECT_GOLDFISH_API UIsPlayerInRange : public UBTService_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UIsPlayerInRange();

	/**
	 * OnBecomeRelevant:
	 * Called when this service becomes active in the Behavior Tree.
	 * Updates blackboard values based on player proximity.
	 * @param behaviorTreeComponent - reference to the AI's Behavior Tree Component
	 * @param nodeMemory - pointer to service memory (internal use by UE)
	 */
	void OnBecomeRelevant(UBehaviorTreeComponent& behaviorTreeComponent, uint8* nodeMemory);
};

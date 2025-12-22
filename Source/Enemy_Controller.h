
#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Enemy_Controller.generated.h"
 
/**
 * AEnemy_Controller:
 * Manages enemy AI behaviors using a Behavior Tree and Blackboard system.
 */
UCLASS()
class PROJECT_GOLDFISH_API AEnemy_Controller : public AAIController
{
	GENERATED_BODY()
	
public:
	// Constructor: Initializes components and finds default assets.
	AEnemy_Controller(FObjectInitializer const& pObjectInit);

	// Overrides
	// Called when the game starts.
	void BeginPlay() override;
	// Called when this controller takes possession of a Pawn.
	void OnPossess(APawn* pPawn) override;

	// Accessor for the Blackboard component.
	class UBlackboardComponent* GetBlackboard() const;

	// Component responsible for running the Behavior Tree logic.
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "AI")
	class UBehaviorTreeComponent* PBehaviorTreeComponent;

	// The Behavior Tree asset to use for this enemy's logic.
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "AI")
	class UBehaviorTree* PBehaviorTree;

private:
	// Component used to store data shared between Behavior Tree nodes.
	class UBlackboardComponent* m_pBlackboard;
};
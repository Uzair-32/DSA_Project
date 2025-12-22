
#include "Enemy_Controller.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "UObject/ConstructorHelpers.h"

AEnemy_Controller::AEnemy_Controller(FObjectInitializer const& pObjectInit)
{
    // Attempt to find the specific Behavior Tree asset in the project content.
    // Note: Hardcoding paths is useful for prototyping but soft references are preferred for production.
    static ConstructorHelpers::FObjectFinder<UBehaviorTree> treeFinder(TEXT("/Script/AIModule.BehaviorTree'/Game/Goldfish/AI/BT_Enemy.BT_Enemy'"));
    if (treeFinder.Succeeded())
    {
        PBehaviorTree = treeFinder.Object;
    }

    // Initialize the Behavior Tree Component.
    PBehaviorTreeComponent = pObjectInit.CreateDefaultSubobject<UBehaviorTreeComponent>(this, TEXT("BehaviorTree Component"));
    
    // Initialize the Blackboard Component.
    m_pBlackboard = pObjectInit.CreateDefaultSubobject<UBlackboardComponent>(this, TEXT("Blackboard Component"));
}

void AEnemy_Controller::BeginPlay()
{
    Super::BeginPlay();

    // Start running the assigned Behavior Tree.
    RunBehaviorTree(PBehaviorTree);
    PBehaviorTreeComponent->StartTree(*PBehaviorTree);
}

void AEnemy_Controller::OnPossess(APawn* a_pPawn)
{
    Super::OnPossess(a_pPawn);

    // When the controller possesses a pawn, initialize the blackboard using the data from the BT asset.
    if (m_pBlackboard && PBehaviorTree)
    {
        m_pBlackboard->InitializeBlackboard(*PBehaviorTree->BlackboardAsset);
    }
}

UBlackboardComponent* AEnemy_Controller::GetBlackboard() const
{
    return m_pBlackboard;
}
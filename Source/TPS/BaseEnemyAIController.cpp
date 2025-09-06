// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseEnemyAIController.h"

#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/BoxComponent.h"

void ABaseEnemyAIController::BeginPlay()
{
    Super::BeginPlay();

    Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (AIBehavior != nullptr)
    {
        RunBehaviorTree(AIBehavior);
        GetBlackboardComponent()->SetValueAsVector(TEXT("Start Location"), GetPawn()->GetActorLocation());

        UBoxComponent* PatrolComponent = GetPawn()->FindComponentByClass<UBoxComponent>();
        if (PatrolComponent != nullptr)
        {
            GetBlackboardComponent()->SetValueAsVector(TEXT("Patrol Location"), PatrolComponent->GetComponentLocation());
        }
    }
}

void ABaseEnemyAIController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    if (LineOfSightTo(Player))
    {
        FVector Spotted = Player->GetActorLocation();
        GetBlackboardComponent()->SetValueAsVector(TEXT("Player Location"), Spotted);
        GetBlackboardComponent()->SetValueAsVector(TEXT("Last Known Player Location"), Spotted);
    }
    else
    {
        GetBlackboardComponent()->ClearValue(TEXT("Player Location"));
    }
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseEnemyAIController.h"

#include "Kismet/GameplayStatics.h"

void ABaseEnemyAIController::BeginPlay()
{
    Super::BeginPlay();
    
    Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
}

void ABaseEnemyAIController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (LineOfSightTo(Player)) 
    {
        SetFocus(Player);
        MoveToActor(Player, 200);
    } 
    else 
    {
        ClearFocus(EAIFocusPriority::Gameplay);
        StopMovement();
    }
}

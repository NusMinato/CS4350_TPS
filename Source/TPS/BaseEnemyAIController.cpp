// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseEnemyAIController.h"

#include "Kismet/GameplayStatics.h"

void ABaseEnemyAIController::BeginPlay()
{
    Super::BeginPlay();
    
    UE_LOG(LogTemp, Warning, TEXT("Worked?"))
    Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    SetFocus(Player);
}

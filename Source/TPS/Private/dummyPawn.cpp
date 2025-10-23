// Fill out your copyright notice in the Description page of Project Settings.


#include "dummyPawn.h"

// Sets default values
AdummyPawn::AdummyPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AdummyPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AdummyPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AdummyPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


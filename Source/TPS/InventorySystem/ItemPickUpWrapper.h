// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemPickUpWrapper.generated.h"

class UItem;
class APlayerCharacter;

UCLASS()
class TPS_API AItemPickUpWrapper : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemPickUpWrapper();

	UPROPERTY(EditAnywhere, Instanced, BlueprintReadOnly, Category = "Item")
	UItem* WrappedItem = nullptr;

	UFUNCTION(BlueprintCallable)
	void OnPickUp(APlayerCharacter* PlayerCharacter);
};

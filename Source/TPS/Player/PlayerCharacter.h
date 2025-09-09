// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../InventorySystem/Items/Item.h"
#include "PlayerCharacter.generated.h"

class UInventoryComponent;
class UItem;

UCLASS()
class TPS_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	TObjectPtr<UInventoryComponent> Inventory; 

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player Stats")
	int32 MaxHealth = 100;


	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Player Stats")
	int32 CurrHealth;

	UFUNCTION(BlueprintCallable)
	void UseItem(UItem* Item) {
		Item->Use(this);
		// blueprint event
		Item->OnUse(this);
	}

};

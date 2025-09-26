// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
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

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Inventory", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UInventoryComponent> Inventory;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player Stats")
	int32 MaxHealth = 100;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Player Stats")
	int32 CurrHealth;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player Stats")
	int32 MaxSanity = 100;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Player Stats")
	int32 CurrSanity;

	UFUNCTION(BlueprintCallable)
	void UseItem(UItem* Item);

	UFUNCTION(BlueprintCallable)
	int32 GetHealth() {
		return this->CurrHealth;
	}

	UFUNCTION(BlueprintCallable)
	void SetHealth(int32 Health) {
		this->CurrHealth = Health;
	}

	UFUNCTION(BlueprintCallable)
	int32 GetSanity() {
		return this->CurrSanity;
	}

	UFUNCTION(BlueprintCallable)
	void SetSanity(int32 Sanity) {
		this->CurrSanity = Sanity;
	}

};

// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerCharacter.h"
#include "../InventorySystem/Items/Item.h"
#include "../InventorySystem/Items/InventoryComponent.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
	Inventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
	Inventory->Capacity = 20;
	CurrHealth = MaxHealth;
}

void APlayerCharacter::UseItem(UItem* Item)
{
	Item->Use(this);
	// blueprint event
	Item->OnUse(this);
}


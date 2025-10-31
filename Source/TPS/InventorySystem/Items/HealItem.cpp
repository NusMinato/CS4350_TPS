// Fill out your copyright notice in the Description page of Project Settings.

#include "HealItem.h"
#include "InventoryComponent.h"
#include "../../Player/PlayerCharacter.h"

UHealItem::UHealItem()
{
	// Make health potions stackable
	bIsStackable = true;
	MaxStackSize = 99;
	ItemDisplayName = FText::FromString("Health Potion");
	UseActionText = FText::FromString("Consume");
}

void UHealItem::Use(APlayerCharacter* PlayerCharacter)
{
	if (!PlayerCharacter || Quantity <= 0)
	{
		return;
	}

	// Apply healing
	int32 NewHealth = PlayerCharacter->GetHealth() + this->HealAmount;
	PlayerCharacter->SetHealth(FMath::Min(NewHealth, PlayerCharacter->MaxHealth));

	// Decrease quantity
	Quantity--;
	
	UE_LOG(LogTemp, Log, TEXT("Used Health Potion. Remaining: %d"), Quantity);

	// Remove from inventory if no more left
	if (Quantity <= 0 && this->OwningInventory)
	{
		this->OwningInventory->RemoveItem(this);
	}
}
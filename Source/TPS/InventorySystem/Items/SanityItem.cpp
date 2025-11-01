// Fill out your copyright notice in the Description page of Project Settings.


#include "SanityItem.h"
#include "InventoryComponent.h"
#include "../Player/PlayerCharacter.h"

USanityItem::USanityItem()
{
	// Make sanity potions stackable
	bIsStackable = true;
	MaxStackSize = 99;
	ItemDisplayName = FText::FromString("Sanity Potion");
	UseActionText = FText::FromString("Consume");
}

void USanityItem::Use(APlayerCharacter* PlayerCharacter)
{
	if (!PlayerCharacter || Quantity <= 0)
	{
		return;
	}

	// Apply sanity restoration
	int32 NewSanity = PlayerCharacter->GetSanity() + this->RestorationAmount;
	PlayerCharacter->SetSanity(FMath::Min(NewSanity, PlayerCharacter->MaxSanity));

	// Decrease quantity
	Quantity--;
	
	UE_LOG(LogTemp, Log, TEXT("Used Sanity Potion. Remaining: %d"), Quantity);

	// Remove from inventory if no more left
	if (Quantity <= 0 && this->OwningInventory)
	{
		this->OwningInventory->RemoveItem(this);
	}
}

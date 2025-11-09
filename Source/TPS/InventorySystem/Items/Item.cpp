// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"
#include "./InventoryComponent.h"

UItem::UItem() {
	this->ItemDisplayName = FText::FromString("Item");
	this->UseActionText = FText::FromString("Use");
}

void UItem::Use(APlayerCharacter* Character)
{
	this->OnUse(Character);
}

bool UItem::CanStackWith(const UItem* OtherItem) const
{
	if (!OtherItem || !bIsStackable || !OtherItem->bIsStackable)
	{
		return false;
	}

	// Items can stack if they're the same class
	return GetClass() == OtherItem->GetClass();
}

int32 UItem::AddQuantity(int32 Amount)
{
	if (!bIsStackable || Amount <= 0)
	{
		return Amount;  // Return all as overflow
	}

	int32 NewQuantity = Quantity + Amount;
	
	if (NewQuantity <= MaxStackSize)
	{
		// All fits in this stack
		Quantity = NewQuantity;
		return 0;  // No overflow
	}
	else
	{
		// Stack is full, return overflow
		Quantity = MaxStackSize;
		return NewQuantity - MaxStackSize;
	}
}

bool UItem::RemoveQuantity(int32 Amount)
{
	if (Amount <= 0 || Amount > Quantity)
	{
		return false;
	}

	Quantity -= Amount;
	this->OwningInventory->OnInventoryUpdated.Broadcast();
	
	return true;
}
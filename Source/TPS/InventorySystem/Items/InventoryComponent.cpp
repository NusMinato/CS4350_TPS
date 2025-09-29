// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"
#include "Item.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	this->Capacity = 20;
}


// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	for (UItem* DefaultItem : this->DefaultItems) {
		AddItem(DefaultItem);
	}
	
}

bool UInventoryComponent::AddItem(UItem* Item)
{
	if (!Item)
	{
		OnInventoryAddFailed.Broadcast(nullptr, FText::FromString(TEXT("Null item")));
		return false;
	}

	if (Items.Num() >= Capacity)
	{
		OnInventoryAddFailed.Broadcast(Item, FText::FromString(TEXT("Inventory is full")));
		return false;
	}

	UItem* ItemForInv = Item->GetOuter() == this
		? Item
		: DuplicateObject<UItem>(Item, this);

	Item->OwningInventory = this;
	Items.Add(Item);

	this->OnInventoryUpdated.Broadcast();

	return true;
}

bool UInventoryComponent::RemoveItem(UItem* Item)
{
	if (Item) {
		Item->OwningInventory = NULL;
		Item->World = NULL;
		this->Items.RemoveSingle(Item);
		this->OnInventoryUpdated.Broadcast();
		return true;
	}

	return false;
}

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

    // Try to stack with existing items first
    if (Item->bIsStackable)
    {
        for (UItem* ExistingItem : Items)
        {
            if (ExistingItem && ExistingItem->CanStackWith(Item))
            {
                int32 Overflow = ExistingItem->AddQuantity(Item->Quantity);
                
                if (Overflow == 0)
                {
                    // All items stacked successfully
                    UE_LOG(LogTemp, Log, TEXT("Stacked %d x %s. New stack: %d"), 
                        Item->Quantity, *Item->ItemDisplayName.ToString(), ExistingItem->Quantity);
                    OnInventoryUpdated.Broadcast();
                    return true;
                }
                else
                {
                    // Partially stacked, existing stack is full
                    // Check if we have room for another stack
                    if (Items.Num() >= Capacity)
                    {
                        OnInventoryAddFailed.Broadcast(Item, FText::FromString(TEXT("Inventory is full")));
                        return false;
                    }
                    
                    // Create new stack with overflow amount
                    UItem* OverflowItem = DuplicateObject<UItem>(Item, this);
                    OverflowItem->Quantity = Overflow;
                    OverflowItem->World = GetWorld();
                    OverflowItem->OwningInventory = this;
                    
                    Items.Add(OverflowItem);
                    OnInventoryUpdated.Broadcast();
                    
                    UE_LOG(LogTemp, Log, TEXT("Created overflow stack: %s x%d"), 
                        *OverflowItem->ItemDisplayName.ToString(), Overflow);
                    return true;
                }
            }
        }
    }

    // Check capacity
    if (Items.Num() >= Capacity)
    {
        OnInventoryAddFailed.Broadcast(Item, FText::FromString(TEXT("Inventory is full")));
        return false;
    }

    // Add as new item
    UItem* ItemForInv = (Item->GetOuter() == this)
        ? Item
        : DuplicateObject<UItem>(Item, this);

    ItemForInv->World = GetWorld();
    ItemForInv->OwningInventory = this;

    Items.Add(ItemForInv);
    OnInventoryUpdated.Broadcast();
    
    UE_LOG(LogTemp, Log, TEXT("Added item: %s x%d"), 
        *ItemForInv->ItemDisplayName.ToString(), ItemForInv->Quantity);
    
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

void UInventoryComponent::Clear()
{
	// Clear all items from inventory
	for (UItem* Item : Items)
	{
		if (Item)
		{
			Item->OwningInventory = nullptr;
			Item->World = nullptr;
		}
	}
	
	Items.Empty();
	OnInventoryUpdated.Broadcast();
	
	UE_LOG(LogTemp, Log, TEXT("Inventory cleared"));
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemPickUpWrapper.h"
#include "Items/Item.h"
#include "Items/InventoryComponent.h"
#include "../Player/PlayerCharacter.h"

AItemPickUpWrapper::AItemPickUpWrapper()
{
}

void AItemPickUpWrapper::OnPickUp(APlayerCharacter* PlayerCharacter)
{
    if (!PlayerCharacter || !PlayerCharacter->Inventory || !WrappedItem) return;

    // Prevent double-pick in the same frame (optional but recommended)
    SetActorEnableCollision(false);

    if (PlayerCharacter->Inventory->AddItem(WrappedItem))
    {
        WrappedItem = nullptr;
        Destroy();
    }
    else
    {
        // Re-enable collision if add failed
        SetActorEnableCollision(true);
    }
}

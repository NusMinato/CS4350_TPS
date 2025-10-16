// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemPickUpWrapper.h"
#include "Items/Item.h"
#include "Items/WeaponItem.h"
#include "Items/InventoryComponent.h"
#include "Items/Interactable.h"
#include "../Player/PlayerCharacter.h"

AItemPickUpWrapper::AItemPickUpWrapper()
{
    this->PickupCollision = CreateDefaultSubobject<USphereComponent>(TEXT("PickupCollision"));
    this->PickupCollision->InitSphereRadius(50.0f);
    this->PickupCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    this->PickupCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
    this->PickupCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    this->PickupCollision->SetGenerateOverlapEvents(true);
    this->RootComponent = this->PickupCollision;
}

void AItemPickUpWrapper::BeginPlay() {
    Super::BeginPlay();

    if (this->bAutoPickUp) {
        this->PickupCollision->OnComponentBeginOverlap.AddDynamic(
            this, &AItemPickUpWrapper::OnOverlapBegin
        );
    }
}

void AItemPickUpWrapper::OnPickUp(APlayerCharacter* PlayerCharacter)
{
    if (!PlayerCharacter || !PlayerCharacter->Inventory || !this->WrappedItem) return;
    SetActorEnableCollision(false); // prevent double pickup


    // Add the item to the inventory
    if (PlayerCharacter->Inventory->AddItem(this->WrappedItem))
    {
        // If it's a weapon, immediately use (equip) it
        if (this->WrappedItem->IsA<UWeaponItem>())
        {
            PlayerCharacter->UseItem(this->WrappedItem);  // This calls UWeaponItem::Use, spawning the actor
        }
        // Remove the item from the wrapper and destroy pickup actor
        this->WrappedItem = nullptr;
        Destroy();
    }
    else
    {
        // If add failed (e.g., inventory full for non-weapons), re-enable collision so it can be picked up later
        SetActorEnableCollision(true);
    }
}


void AItemPickUpWrapper::OnOverlapBegin(UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor, UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    // Check if overlapping a player character
    APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
    if (Player && this->bAutoPickUp)
    {
        this->OnPickUp(Player);
    }
}

void AItemPickUpWrapper::Interact_Implementation(APawn* InstigatorPawn)
{
    // When the player presses E and line trace hits this item, this function is called.
    APlayerCharacter* Player = Cast<APlayerCharacter>(InstigatorPawn);
    if (Player)
    {
        this->OnPickUp(Player);
    }
}

FText AItemPickUpWrapper::GetInteractText_Implementation() const
{
    if (this->WrappedItem)
    {
        // e.g. "Press E to pick up Pistol"
        FText itemName = this->WrappedItem->ItemDisplayName;
        return FText::Format(NSLOCTEXT("Interact", "PickupPrompt", "Press E to pick up {0}"), itemName);
    }
    // Default text if no item data
    return FText::FromString("Press E to interact");
}
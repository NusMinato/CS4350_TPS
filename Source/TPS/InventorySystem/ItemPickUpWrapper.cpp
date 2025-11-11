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
    this->RootComponent = this->PickupCollision;
    
    // Enable tick to check if player is looking at this item
    PrimaryActorTick.bCanEverTick = true;
}

void AItemPickUpWrapper::BeginPlay() {
    Super::BeginPlay();

    if (this->bAutoPickUp) {
        this->PickupCollision->OnComponentBeginOverlap.AddDynamic(
            this, &AItemPickUpWrapper::OnOverlapBegin
        );
    }

    // Make sure widget is hidden at start
}

void AItemPickUpWrapper::OnPickUp(APlayerCharacter* PlayerCharacter)
{
    if (!PlayerCharacter || !PlayerCharacter->Inventory || !this->WrappedItem) return;
    SetActorEnableCollision(false); // prevent double pickup

    // 1) Duplicate into a safe owner (the InventoryComponent) BEFORE destroying the wrapper
    UItem* NewItem = DuplicateObject<UItem>(this->WrappedItem, PlayerCharacter->Inventory);

    // 2) Add the duplicated item
    if (PlayerCharacter->Inventory->AddItem(NewItem))
    {
        // 3) Auto-equip weapons using the duplicated pointer (not the old one)
        if (NewItem->IsA<UWeaponItem>())
        {
            PlayerCharacter->UseItem(NewItem);
        }

        // Safe to remove the pickup actor now
        this->WrappedItem = nullptr;
        Destroy();
    }
    else
    {
        // If add failed, allow another try
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
        // e.g. "Press F to pick up Pistol"
        FText itemName = this->WrappedItem->ItemDisplayName;
        return FText::Format(NSLOCTEXT("Interact", "PickupPrompt", "Press F to pick up {0}"), itemName);
    }
    // Default text if no item data
    return FText::FromString("Press F to interact");
}
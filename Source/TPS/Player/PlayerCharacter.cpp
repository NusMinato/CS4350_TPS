// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerCharacter.h"
#include "Camera/PlayerCameraManager.h"
#include "../InventorySystem/ItemPickUpWrapper.h"
#include "../InventorySystem/Items/Interactable.h"
#include "../InventorySystem/Items/InventoryComponent.h"
#include "../InventorySystem/Items/Item.h"
#include "../InventorySystem/Items/WeaponItem.h"
#include "../InventorySystem/Weapon/Weapon.h"
#include "../InventorySystem/Weapon/WeaponActor.h"
#include "GameFramework/PlayerController.h"
#include "DrawDebugHelpers.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
    Inventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
    Inventory->Capacity = 20;
    CurrHealth = MaxHealth;
    CurrSanity = MaxSanity;
}

void APlayerCharacter::UseItem(UItem* Item)
{
	if (!Item)
	{
		return;
	}
	
	Item->Use(this);
	// blueprint event
	Item->OnUse(this);
}

FVector APlayerCharacter::GetLookAtPoint() const
{
    const float Range = 15000.f;

    const FVector CamLoc = GetPawnViewLocation();          // camera/eyes
    const FRotator AimRot = GetBaseAimRotation();           // controller aim
    const FVector End = CamLoc + AimRot.Vector() * Range;

    FHitResult Hit;
    FCollisionQueryParams Params(SCENE_QUERY_STAT(GetLookAtPoint), /*bTraceComplex=*/true, this);
    Params.AddIgnoredActor(this);

    if (GetWorld()->LineTraceSingleByChannel(Hit, CamLoc, End, ECC_Visibility, Params) && Hit.bBlockingHit)
    {
        return Hit.Location;
    }
    return End;
}

void APlayerCharacter::EquipWeapon(UWeaponItem* Item, AWeaponActor* WeaponActor)
{
    if (!Item || !WeaponActor) return;

    TObjectPtr<AWeaponActor> SlotActor = nullptr;
    TObjectPtr<UWeaponItem> SlotItem = nullptr;

    // Get the current weapon in the slot
    switch (Item->WeaponType)
    {
    case EMyWeaponType::LongGun:
        SlotActor = PrimaryWeapon;
        SlotItem = PrimaryWeaponItem;
        break;
    case EMyWeaponType::Pistol:
        SlotActor = SecondaryWeapon;
        SlotItem = SecondaryWeaponItem;
        break;
    case EMyWeaponType::ColdWeapon:
        SlotActor = MeleeWeapon;
        SlotItem = MeleeWeaponItem;
        break;
    default:
        return;
    }

    // Check for desync: if we have an item without actor or actor without item, clean up the slot
    if ((SlotItem && !SlotActor) || (SlotActor && !SlotItem))
    {
        // Clear the slot to fix inconsistent state
        switch (Item->WeaponType)
        {
        case EMyWeaponType::LongGun:
            PrimaryWeapon = nullptr;
            PrimaryWeaponItem = nullptr;
            break;
        case EMyWeaponType::Pistol:
            SecondaryWeapon = nullptr;
            SecondaryWeaponItem = nullptr;
            break;
        case EMyWeaponType::ColdWeapon:
            MeleeWeapon = nullptr;
            MeleeWeaponItem = nullptr;
            break;
        }
        SlotActor = nullptr;
        SlotItem = nullptr;
    }

    // If clicking the *same* item that's already equipped → unequip it
    if (SlotItem == Item && SlotActor)
    {
        Item->UnequipWeapon(this);
        return;
    }

    // Now equip the new weapon
    if (SlotItem && SlotItem != Item) {
        SlotItem->UnequipWeapon(this);
    }
    switch (Item->WeaponType)
    {
    case EMyWeaponType::LongGun:
        PrimaryWeapon = WeaponActor;
        PrimaryWeaponItem = Item;
        break;
    case EMyWeaponType::Pistol:
        SecondaryWeapon = WeaponActor;
        SecondaryWeaponItem = Item;
        break;
    case EMyWeaponType::ColdWeapon:
        MeleeWeapon = WeaponActor;
        MeleeWeaponItem = Item;
        break;
    }

    // Notify Blueprint to handle attachment and visibility
    BP_OnWeaponEquipped(Item, WeaponActor);
    OnEquippedWeaponUpdated.Broadcast();
}

void APlayerCharacter::OnWeaponDropped(UWeaponItem* WeaponItem)
{
    if (!WeaponItem || !Inventory) return;

    // Find and clear the weapon from the appropriate slot
    AWeaponActor* WeaponActor = nullptr;
    
    if (WeaponItem == PrimaryWeaponItem)
    {
        WeaponActor = PrimaryWeapon;
        PrimaryWeapon = nullptr;
        PrimaryWeaponItem = nullptr;
    }
    else if (WeaponItem == SecondaryWeaponItem)
    {
        WeaponActor = SecondaryWeapon;
        SecondaryWeapon = nullptr;
        SecondaryWeaponItem = nullptr;
    }
    else if (WeaponItem == MeleeWeaponItem)
    {
        WeaponActor = MeleeWeapon;
        MeleeWeapon = nullptr;
        MeleeWeaponItem = nullptr;
    }

    // Notify Blueprint to handle drop (detach, enable physics, etc.)
    if (WeaponActor)
    {
        BP_OnWeaponDropped(WeaponItem, WeaponActor);
        WeaponActor->Drop();
    }
    
    // Remove from inventory
    Inventory->RemoveItem(WeaponItem);
    OnEquippedWeaponUpdated.Broadcast();
}

void APlayerCharacter::UnequipWeapon(UWeaponItem* WeaponItem)
{
    if (!WeaponItem) return;

    AWeaponActor* WeaponActor = nullptr;

    if (WeaponItem == this->ActiveWeaponItem) {
        WeaponActor = this->ActiveWeapon;
        
        // Sync data back from WeaponActor to WeaponItem before destroying
        if (WeaponActor && WeaponItem) {
            WeaponItem->CurrentAmmo = WeaponActor->CurrentAmmo;
            WeaponItem->MaxAmmo = WeaponActor->MaxAmmo;
            WeaponItem->Damage = WeaponActor->Damage;
            WeaponItem->SanityCost = WeaponActor->SanityCost;
            WeaponItem->WeaponType = WeaponActor->WeaponType;
            WeaponItem->IsEquipped = false;
            WeaponItem->SetRuntimeActor(nullptr);
        }
        
        // Destroy the weapon actor
        if (WeaponActor) {
            WeaponActor->Destroy();
        }
        
        OnActiveWeaponUnequipped.Broadcast();
        this->ActiveWeaponItem = nullptr;
        this->ActiveWeapon = nullptr;
    }

    if (WeaponItem == this->PrimaryWeaponItem) {
        WeaponActor = this->PrimaryWeapon;
        this->PrimaryWeapon = nullptr;
        this->PrimaryWeaponItem = nullptr;
    }
    else if (WeaponItem == this->SecondaryWeaponItem) {
        WeaponActor = this->SecondaryWeapon;
        this->SecondaryWeapon = nullptr;
        this->SecondaryWeaponItem = nullptr;
    }
    else if (WeaponItem == this->MeleeWeaponItem) {
        WeaponActor = this->MeleeWeapon;
        this->MeleeWeapon = nullptr;
        this->MeleeWeaponItem = nullptr;
    }

    // Notify Blueprint to handle detachment and visibility
    if (WeaponActor) {
        BP_OnWeaponUnequipped(WeaponItem, WeaponActor);
    }
    OnEquippedWeaponUpdated.Broadcast();
}

TArray<UWeaponItem*> APlayerCharacter::GetAllWeapons()
{
    TArray<UWeaponItem*> Weapons;
    if (this->PrimaryWeapon) {
        Weapons.Add(this->PrimaryWeaponItem);
    }
    if (this->SecondaryWeapon) {
        Weapons.Add(this->SecondaryWeaponItem);
    }
    if (this->MeleeWeapon) {
        Weapons.Add(this->MeleeWeaponItem);
    }
    return Weapons;
}

void APlayerCharacter::SetActiveWeapon(UWeaponItem* WeaponItem)
{
    if (!WeaponItem) 
    {
        UE_LOG(LogTemp, Warning, TEXT("SetActiveWeapon: Clearing active weapon"));
        // Clear active weapon
        ActiveWeaponItem = nullptr;
        ActiveWeapon = nullptr;
        OnEquippedWeaponUpdated.Broadcast();
        return;
    }

    // Set the active weapon item
    ActiveWeaponItem = WeaponItem;
    
    // Get the RuntimeActor from the weapon item and set it as ActiveWeapon
    ActiveWeapon = WeaponItem->GetRuntimeActor();
    
    UE_LOG(LogTemp, Warning, TEXT("SetActiveWeapon: ActiveWeaponItem=%s, ActiveWeapon=%s"), 
        *WeaponItem->ItemDisplayName.ToString(),
        ActiveWeapon ? *ActiveWeapon->GetName() : TEXT("NULL"));
    
    if (ActiveWeapon)
    {
        UE_LOG(LogTemp, Warning, TEXT("ActiveWeapon Stats: MaxAmmo=%d, CurrentAmmo=%d, Damage=%d"), 
            ActiveWeapon->MaxAmmo, ActiveWeapon->CurrentAmmo, ActiveWeapon->Damage);
    }
    
    // Broadcast update event
    OnEquippedWeaponUpdated.Broadcast();
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    // Assume "Interact" action mapping is bound to E in project settings:
    PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &APlayerCharacter::Interact);
}

void APlayerCharacter::Interact()
{
    const float InteractRange = 500.f;    // Maximum interact distance
    const float InteractRadius = 100.f;    // Larger sphere radius for easier pickup

    // Start from camera (what you're seeing) and sweep forward
    const FVector  CamStart = GetPawnViewLocation();
    const FRotator AimRot = GetBaseAimRotation();
    const FVector  CamEnd = CamStart + AimRot.Vector() * InteractRange;

    // Sphere sweep from camera forward - picks up what you're looking at
    FHitResult Hit;
    FCollisionQueryParams Params(SCENE_QUERY_STAT(Interact_Sweep), /*bTraceComplex=*/true, this);
    Params.AddIgnoredActor(this);
    TArray<AActor*> AttachedActors;
    GetAttachedActors(AttachedActors);
    Params.AddIgnoredActors(AttachedActors);

    // Use all object types for interaction
    FCollisionObjectQueryParams ObjParams;
    // If you want specific channels: ObjParams.AddObjectTypesToQuery(ECC_WorldDynamic);

    const bool bHit = GetWorld()->SweepSingleByObjectType(
        Hit,
        CamStart,
        CamEnd,
        FQuat::Identity,
        ObjParams,
        FCollisionShape::MakeSphere(InteractRadius),
        Params
    );

// #if !(UE_BUILD_SHIPPING)
//     // Draw the interact sweep line from camera
//     DrawDebugLine(GetWorld(), CamStart, CamEnd, 
//         bHit ? FColor::Green : FColor::Red, false, 1.f, 0, 2.f);
    
//     // Draw sphere at start (camera position)
//     DrawDebugSphere(GetWorld(), CamStart, InteractRadius, 12, 
//         FColor::Yellow, false, 1.f, 0, 1.f);
    
//     // Draw sphere at max reach
//     DrawDebugSphere(GetWorld(), CamEnd, InteractRadius, 12, 
//         FColor::Blue, false, 1.f, 0, 1.f);
    
//     // If hit something, show the hit point
//     if (bHit)
//     {
//         DrawDebugSphere(GetWorld(), Hit.ImpactPoint, 10.f, 12, 
//             FColor::Orange, false, 1.f, 0, 2.f);
//         DrawDebugString(GetWorld(), Hit.ImpactPoint + FVector(0, 0, 30.f), 
//             Hit.GetActor() ? Hit.GetActor()->GetName() : TEXT("Unknown"), 
//             nullptr, FColor::White, 1.f);
//     }
// #endif

    if (bHit && Hit.GetActor())
    {
        AActor* HitActor = Hit.GetActor();
        if (HitActor->GetClass()->ImplementsInterface(UInteractable::StaticClass()))
        {
            IInteractable::Execute_Interact(HitActor, this);
        }
    }
}

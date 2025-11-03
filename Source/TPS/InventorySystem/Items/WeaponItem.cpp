// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponItem.h"
#include "../Player/PlayerCharacter.h"
#include "../Weapon/WeaponActor.h"


void UWeaponItem::Use(APlayerCharacter* PC) {
	if (this->IsEquipped) {
		this->UnequipWeapon(PC);
	} else {
		this->EquipWeapon(PC);
	}
}

void UWeaponItem::DropWeapon(APlayerCharacter* PlayerCharacter) {
	if (!PlayerCharacter) return;

	// Let the character handle slot cleanup and actor dropping
	PlayerCharacter->OnWeaponDropped(this);
	
	// Clear item state
	this->IsEquipped = false;
	this->SetRuntimeActor(nullptr);
}

void UWeaponItem::EquipWeapon(APlayerCharacter* PlayerCharacter) {
	if (!PlayerCharacter || !this->WeaponActorClass) 
	{
		UE_LOG(LogTemp, Error, TEXT("EquipWeapon FAILED: PlayerCharacter=%s, WeaponActorClass=%s"), 
			PlayerCharacter ? TEXT("Valid") : TEXT("NULL"),
			this->WeaponActorClass ? TEXT("Valid") : TEXT("NULL"));
		return;
	}

	this->IsEquipped = true;
	PlayerCharacter->EquipWeapon(this);
}

void UWeaponItem::UnequipWeapon(APlayerCharacter* PlayerCharacter) {
	if (!PlayerCharacter) return;
	this->IsEquipped = false;
	// Blueprint will handle detachment and visibility
	PlayerCharacter->UnequipWeapon(this);
}

void UWeaponItem::DestroyRuntimeActor()
{
	AWeaponActor* WA = this->GetRuntimeActor();
	if (!WA) return;
	this->SetWeaponProperties();
	WA->Destroy();
	this->SetRuntimeActor(nullptr);
}

void UWeaponItem::SpawnRuntimeActor()
{
	AWeaponActor* WA = this->GetRuntimeActor();
	if (IsValid(WA)) return;
	this->SetRuntimeActor(nullptr);

	UWorld* WorldCtx = this->GetWorld();
	WA = WorldCtx->SpawnActor<AWeaponActor>(this->WeaponActorClass);

	if (!WA) {
		UE_LOG(LogTemp, Error, TEXT("EquipWeapon FAILED: World context is NULL"));
	}

	WA->SourceItem = this;
	WA->CurrentAmmo = this->CurrentAmmo;
	WA->MaxAmmo = this->MaxAmmo;
	WA->Damage = this->Damage;
	WA->SanityCost = this->SanityCost;

	this->SetRuntimeActor(WA);
}

void UWeaponItem::EquipRuntimeActor(APlayerCharacter* PC)
{
	if (!PC) return;

	// 1) Ensure we have a live runtime actor
	if (!IsValid(RuntimeActor)) {
		// Prefer passing PC so you can set owner/instigator and pick the right world
		this->SpawnRuntimeActor();     // <- implement this to set Owner=PC, etc.
		AWeaponActor* NewWA = this->GetRuntimeActor();
		if (!NewWA) return;
		RuntimeActor = NewWA;
		// copy properties (ammo, damage, etc.) here
	}

	// 2) Now make it the active weapon (pulls RuntimeActor from the item)
	PC->SetActiveWeapon(this);
}

void UWeaponItem::UnEquipRuntimeActor(APlayerCharacter* PC)
{
	if (!PC) return;

	// 1) Stop using it first so UI/abilities stop touching it
	PC->SetActiveWeapon(nullptr);

	// 2) Then safely destroy and CLEAR THE POINTER
	if (IsValid(RuntimeActor)) {
		this->DestroyRuntimeActor();
	}
}

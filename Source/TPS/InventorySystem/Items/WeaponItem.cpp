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
	this->RuntimeActor = nullptr;
}

void UWeaponItem::EquipWeapon(APlayerCharacter* PlayerCharacter) {
	if (!PlayerCharacter || !this->WeaponActorClass) return;

	UWorld* WorldCtx = PlayerCharacter->GetWorld();
	if (!WorldCtx) return;
	
	AWeaponActor* SpawnedWeapon = this->RuntimeActor;
	if (!SpawnedWeapon) {
		SpawnedWeapon = WorldCtx->SpawnActor<AWeaponActor>(this->WeaponActorClass);
		if (!SpawnedWeapon) return;
		
		SpawnedWeapon->SourceItem = this;
		SpawnedWeapon->OwningCharacter = PlayerCharacter;
		SpawnedWeapon->SetOwner(PlayerCharacter);

		SpawnedWeapon->CurrentAmmo = this->CurrentAmmo;
		SpawnedWeapon->MaxAmmo = this->MaxAmmo;
		SpawnedWeapon->SanityCost = this->SanityCost;
		SpawnedWeapon->WeaponType = this->WeaponType;
		this->RuntimeActor = SpawnedWeapon;
	}
	// Blueprint will handle attachment and visibility

	this->IsEquipped = true;
	PlayerCharacter->EquipWeapon(this, SpawnedWeapon);
}

void UWeaponItem::UnequipWeapon(APlayerCharacter* PlayerCharacter) {
	this->IsEquipped = false;
	// Blueprint will handle detachment and visibility
	PlayerCharacter->UnequipWeapon(this);
}

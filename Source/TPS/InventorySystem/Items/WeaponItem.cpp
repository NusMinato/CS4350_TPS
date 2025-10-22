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

	UWorld* WorldCtx = PlayerCharacter->GetWorld();
	if (!WorldCtx) 
	{
		UE_LOG(LogTemp, Error, TEXT("EquipWeapon FAILED: World context is NULL"));
		return;
	}
	
	AWeaponActor* SpawnedWeapon = this->RuntimeActor;
	if (!SpawnedWeapon) {
		UE_LOG(LogTemp, Warning, TEXT("Spawning new weapon actor from class: %s"), *WeaponActorClass->GetName());
		UE_LOG(LogTemp, Warning, TEXT("WeaponItem DATA BEFORE SPAWN: MaxAmmo=%d, CurrentAmmo=%d, Damage=%d, SanityCost=%d"), 
			this->MaxAmmo, this->CurrentAmmo, this->Damage, this->SanityCost);
		
		SpawnedWeapon = WorldCtx->SpawnActor<AWeaponActor>(this->WeaponActorClass);
		if (!SpawnedWeapon) 
		{
			UE_LOG(LogTemp, Error, TEXT("SpawnActor FAILED! Could not spawn weapon actor."));
			return;
		}
		
		UE_LOG(LogTemp, Warning, TEXT("Weapon Actor spawned successfully: %s"), *SpawnedWeapon->GetName());
		
		SpawnedWeapon->SourceItem = this;
		SpawnedWeapon->OwningCharacter = PlayerCharacter;
		SpawnedWeapon->SetOwner(PlayerCharacter);

		SpawnedWeapon->CurrentAmmo = this->CurrentAmmo;
		SpawnedWeapon->MaxAmmo = this->MaxAmmo;
		SpawnedWeapon->SanityCost = this->SanityCost;
		SpawnedWeapon->WeaponType = this->WeaponType;
		SpawnedWeapon->Damage = this->Damage;
		
		UE_LOG(LogTemp, Warning, TEXT("WeaponActor DATA AFTER COPY: MaxAmmo=%d, CurrentAmmo=%d, Damage=%d, SanityCost=%d"), 
			SpawnedWeapon->MaxAmmo, SpawnedWeapon->CurrentAmmo, SpawnedWeapon->Damage, SpawnedWeapon->SanityCost);
		
		this->SetRuntimeActor(SpawnedWeapon);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Re-equipping existing weapon actor: %s"), *SpawnedWeapon->GetName());
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

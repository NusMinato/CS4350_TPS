// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "../Weapon/EMyWeaponType.h"
#include "../Weapon/WeaponActor.h"
#include "WeaponItem.generated.h"

class APlayerCharacter;
class AWeaponActor;

/**
 * 
 */

UCLASS()
class TPS_API UWeaponItem : public UItem
{

	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Info")
	TSubclassOf<AWeaponActor> WeaponActorClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Info")
	int32 SanityCost;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Info")
	int32 MaxAmmo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Info")
	int32 CurrentAmmo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Info")
	EMyWeaponType WeaponType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Info")
	int32 Damage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Info")
	bool IsEquipped = false;
	
	virtual void Use(APlayerCharacter* PlayerCharacter) override;
	void DropWeapon(APlayerCharacter* PlayerCharacter);
	void EquipWeapon(APlayerCharacter* PlayerCharacter);
	void UnequipWeapon(APlayerCharacter* PlayerCharacter);

	// Get the spawned weapon actor (can return nullptr if not spawned)
	UFUNCTION(BlueprintPure, Category = "Weapon")
	AWeaponActor* GetRuntimeActor() const { return RuntimeActor; }

	// Check if weapon has a spawned actor
	UFUNCTION(BlueprintPure, Category = "Weapon")
	bool HasRuntimeActor() const { return RuntimeActor != nullptr; }

	// Update weapon properties and sync to RuntimeActor if it exists
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void SetWeaponProperties() 
	{ 
		// Sync to RuntimeActor if it exists
		if (RuntimeActor) 
		{
			this->CurrentAmmo = RuntimeActor->CurrentAmmo;
			this->MaxAmmo = RuntimeActor->MaxAmmo;
			this->SanityCost = RuntimeActor->SanityCost;
			this->Damage = RuntimeActor->Damage;
		}
	}

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void DestroyRuntimeActor();
	
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void SpawnRuntimeActor();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void EquipRuntimeActor(APlayerCharacter* PC);

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void UnEquipRuntimeActor(APlayerCharacter* PC);

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void SetRuntimeActor(AWeaponActor* NewActor) { RuntimeActor = NewActor; }
protected:

	UPROPERTY(Transient)
	TObjectPtr<AWeaponActor> RuntimeActor = nullptr;

};
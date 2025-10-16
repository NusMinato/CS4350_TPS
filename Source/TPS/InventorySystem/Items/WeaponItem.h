// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "../Weapon/EMyWeaponType.h"
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
protected:

	UPROPERTY(Transient)
	TObjectPtr<AWeaponActor> RuntimeActor = nullptr;

};
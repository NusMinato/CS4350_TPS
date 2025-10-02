// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "WeaponItem.generated.h"

class APlayerCharacter;

/**
 * 
 */
UCLASS()
class TPS_API UWeaponItem : public UItem
{

	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sanity")
	int32 SanityCost;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Info")
	int32 MaxAmmo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sanity")
	int32 CurrentAmmo;

	UFUNCTION(BlueprintCallable)
	virtual void Relaod(APlayerCharacter* PC);

protected:

	virtual void Use(APlayerCharacter* PlayerCharacter) override;

};
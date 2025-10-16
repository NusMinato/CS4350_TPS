// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.h"
#include "EMyWeaponType.h"
#include "WeaponActor.generated.h"

class APlayerCharacter;
class UWeaponItem;

UCLASS(BlueprintType, Blueprintable)
class TPS_API AWeaponActor : public AActor, public IWeapon
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponActor();

	UPROPERTY(BlueprintReadWrite, Category = "Owning Character")
	TWeakObjectPtr<APlayerCharacter> OwningCharacter;

	UPROPERTY(BlueprintReadWrite, Category = "Item")
	TWeakObjectPtr<UWeaponItem> SourceItem;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Info")
	int32 SanityCost;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Info")
	int32 MaxAmmo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly , Category = "Weapon Info")
	int32 CurrentAmmo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Info")
	EMyWeaponType WeaponType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Info")
	int32 Damage;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Weapon")
	void Drop();
};

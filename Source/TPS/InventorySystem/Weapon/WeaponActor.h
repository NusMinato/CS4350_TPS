// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.h"
#include "EMyWeaponType.h"
#include "WeaponActor.generated.h"

class APlayerCharacter;
class UWeaponItem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReloadComplete);

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

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon Info")
	int32 SanityCost;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon Info")
	int32 MaxAmmo;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon Info")
	int32 CurrentAmmo;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon Info")
	EMyWeaponType WeaponType;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon Info")
	int32 Damage;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Weapon")
	void Drop();

	UPROPERTY(BlueprintAssignable, Category = "Weapon")
	FOnReloadComplete OnReloadComplete;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void BroadcastReloadComplete();
};

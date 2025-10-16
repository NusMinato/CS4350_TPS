// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

class AWeaponActor;
class UInventoryComponent;
class UItem;
class UWeaponItem;

UCLASS()
class TPS_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Inventory", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UInventoryComponent> Inventory;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInventoryComponent> WeaponInventory;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<AWeaponActor> PrimaryWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<AWeaponActor> SecondaryWeapon;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<AWeaponActor> MeleeWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UWeaponItem> PrimaryWeaponItem;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UWeaponItem> SecondaryWeaponItem;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UWeaponItem> MeleeWeaponItem;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player Stats")
	int32 MaxHealth = 100;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Player Stats")
	int32 CurrHealth;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player Stats")
	int32 MaxSanity = 100;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Player Stats")
	int32 CurrSanity;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Interaction")
	float InteractDistance = 500.f;

	// Currently focused interactable item (in line of sight)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	AActor* FocusedItem;

	// Function to perform interaction trace and handle results
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void Interact();

	UFUNCTION(BlueprintCallable)
	void UseItem(UItem* Item);

	UFUNCTION(BlueprintCallable)
	int32 GetHealth() {
		return this->CurrHealth;
	}

	UFUNCTION(BlueprintCallable)
	void SetHealth(int32 Health) {
		this->CurrHealth = FMath::Clamp(Health, 0, MaxHealth);
	}

	UFUNCTION(BlueprintCallable)
	int32 GetSanity() {
		return this->CurrSanity;
	}

	UFUNCTION(BlueprintCallable)
	void SetSanity(int32 Sanity) {
		this->CurrSanity = FMath::Clamp(Sanity, 0, MaxSanity);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Aim")
	FVector GetLookAtPoint() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void EquipWeapon(UWeaponItem* Item, AWeaponActor* WeaponActor);

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void OnWeaponDropped(UWeaponItem* WeaponItem);

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void UnequipWeapon(UWeaponItem* WeaponItem);

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	TArray<AWeaponActor*> GetAllWeapons();

	// Blueprint events for weapon attachment/visibility management
	UFUNCTION(BlueprintImplementableEvent, Category = "Weapon")
	void BP_OnWeaponEquipped(UWeaponItem* WeaponItem, AWeaponActor* WeaponActor);

	UFUNCTION(BlueprintImplementableEvent, Category = "Weapon")
	void BP_OnWeaponUnequipped(UWeaponItem* WeaponItem, AWeaponActor* WeaponActor);

	UFUNCTION(BlueprintImplementableEvent, Category = "Weapon")
	void BP_OnWeaponDropped(UWeaponItem* WeaponItem, AWeaponActor* WeaponActor);

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	/** Cache so we only update UI when the focused actor changes */
	TWeakObjectPtr<AActor> LastFocusedInteractable;

};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "InventoryComponent.h"
#include "../../Player/PlayerCharacter.h"
#include "Item.generated.h"

class APlayerCharacter;
class InventoryComponent;

/**
 * 
 */
UCLASS(Abstract, BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class TPS_API UItem : public UObject
{
	GENERATED_BODY()
	
public:
	UItem();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item")
	FText UseActionText;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item")
	TObjectPtr<UStaticMesh> PickupMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item")
	TObjectPtr<UTexture2D> Thumbnail;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item")
	FText ItemDisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item", meta=(MultiLine = true))
	FText ItemDescription;

	UPROPERTY()
	InventoryComponent* OwningInventory;

	virtual void Use(APlayerCharacter* Character) PURE_VIRTUAL(UItem, );

	UFUNCTION(BlueprintImplementableEvent, Category = "Item")
	void OnUse(APlayerCharacter* Character);

};

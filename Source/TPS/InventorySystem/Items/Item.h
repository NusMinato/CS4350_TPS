// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "InventoryComponent.h"
#include "Item.generated.h"

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
	TObjectPtr<UInventoryComponent> OwningInventory;

	//virtual void Use(TObjectPtr<AInventorySystemCharacter> Character) PURE_VIRTUAL(UItem, );

	//UFUNCTION(BlueprintImplementableEvent)
	//void OnUse(TObjectPtr<AInventorySystemCharacter> Character);

};

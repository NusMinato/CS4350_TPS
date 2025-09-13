// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Item.generated.h"

class APlayerCharacter;
class UInventoryComponent;

/**
 * 
 */
UCLASS(Abstract, BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class TPS_API UItem : public UObject
{
	GENERATED_BODY()
	
public:
	UItem();

	virtual class UWorld* GetWorld() const { return World; };

	UPROPERTY(Transient)
	class UWorld* World;

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item")
	UInventoryComponent* OwningInventory;

	virtual void Use(APlayerCharacter* Character);

	UFUNCTION(BlueprintImplementableEvent, Category = "Item")
	void OnUse(APlayerCharacter* Character);

};

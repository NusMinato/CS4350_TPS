// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

class UItem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryUpdated);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInventoryAddFailed, UItem*, Item, FText, Reason);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TPS_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventoryComponent();

	// Called when the game starts
	virtual void BeginPlay() override;

	bool AddItem(UItem* Item);
	bool RemoveItem(UItem* Item);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Inventory", Instanced)
	TArray<TObjectPtr<UItem>> DefaultItems;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	int32 Capacity;

	UPROPERTY(Transient)
	TObjectPtr<UWorld> World;

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryUpdated OnInventoryUpdated;

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryAddFailed OnInventoryAddFailed;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite, Category = "Inventory", Instanced)
	TArray<TObjectPtr<UItem>> Items;
};

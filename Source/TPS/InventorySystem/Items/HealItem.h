// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Item.h"
#include "HealItem.generated.h"

/**
 * 
 */
UCLASS()
class TPS_API UHealItem : public UItem
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Healing Amount", meta = (ClampMin = "10"))
	int32 HealAmount = 10;

protected:

	virtual void Use(APlayerCharacter* PlayerCharacter) override;
	
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "SanityItem.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced)
class TPS_API USanityItem : public UItem
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sanity restoration Amount", meta = (ClampMin = "10"))
	int32 RestorationAmount = 10;

protected:
	virtual void Use(APlayerCharacter* PlayerCharacter) override;
};

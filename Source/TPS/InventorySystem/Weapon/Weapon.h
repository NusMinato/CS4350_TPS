// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Weapon.generated.h"


// Make the interface implementable in Blueprints and exported from your module.
UINTERFACE(BlueprintType)
class TPS_API UWeapon : public UInterface
{
    GENERATED_BODY()
};

class TPS_API IWeapon
{
    GENERATED_BODY()
public:

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Weapon")
    void FireReleased();

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Weapon")
    void Drop();
};
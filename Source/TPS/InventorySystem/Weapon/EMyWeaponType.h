// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EMyWeaponType : uint8
{
    LongGun     UMETA(DisplayName = "Long Gun"),
    Pistol      UMETA(DisplayName = "Pistol"),
    ColdWeapon  UMETA(DisplayName = "Cold Weapon"),
};

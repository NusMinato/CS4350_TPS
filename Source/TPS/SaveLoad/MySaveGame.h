// MySaveGame.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "../InventorySystem/Weapon/EMyWeaponType.h"
#include "MySaveGame.generated.h"

// ========== STRUCTS FIRST (ALWAYS!) ==========

USTRUCT(BlueprintType)
struct FWeaponSaveData
{
    GENERATED_BODY()
    
    UPROPERTY()
    FString WeaponClassName;
    
    UPROPERTY()
    int32 CurrentAmmo = 0;
    
    UPROPERTY()
    int32 MaxAmmo = 0;
    
    UPROPERTY()
    int32 Damage = 0;
    
    UPROPERTY()
    int32 SanityCost = 0;
    
    UPROPERTY()
    uint8 WeaponType = 0;
    
    UPROPERTY()
    bool bIsEquipped = false;
};

USTRUCT(BlueprintType)
struct FItemSaveData
{
    GENERATED_BODY()
    
    UPROPERTY()
    FString ItemClassName;
    
    UPROPERTY()
    int32 Quantity = 1;  // Number of items of this type
};

// ========== SAVE GAME CLASS ==========

UCLASS()
class TPS_API UMySaveGame : public USaveGame
{
    GENERATED_BODY()

public:
    // ========== Save Metadata ==========
    UPROPERTY()
    FDateTime SaveTimestamp;  // When was this saved
    
    UPROPERTY()
    int32 SaveVersion = 1;
    
    // ========== Level Progression ==========
    UPROPERTY()
    FString CurrentLevelName;
    
    UPROPERTY()
    TArray<FString> UnlockedLevels;
    
    UPROPERTY()
    int32 HighestLevelReached = 0;
    
    // ========== Player Inventory ==========
    UPROPERTY()
    TArray<FWeaponSaveData> WeaponData;
    
    UPROPERTY()
    TArray<FItemSaveData> ItemData;
    
    UPROPERTY()
    FString ActiveWeaponClassName;
    
    UPROPERTY()
    int32 ActiveWeaponIndex = -1; // Index of the active weapon in the WeaponData array
    
    // ========== Player Stats ==========
    UPROPERTY()
    int32 CurrentHealth = 100;
    
    UPROPERTY()
    int32 CurrentSanity = 100;
};
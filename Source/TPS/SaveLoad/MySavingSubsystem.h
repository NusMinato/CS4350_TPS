// MySavingSubsystem.h
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MySaveGame.h"
#include "../Player/PlayerCharacter.h"
#include "../InventorySystem/Items/InventoryComponent.h"
#include "../InventorySystem/Items/Item.h"
#include "../InventorySystem/Items/WeaponItem.h"
#include "../InventorySystem/Weapon/WeaponActor.h"
#include "MySavingSubsystem.generated.h"

UCLASS()
class TPS_API UMySavingSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

private:
    // Single slot name - always the same
    static constexpr const TCHAR* SaveSlotName = TEXT("GameSave");
    static constexpr int32 UserIndex = 0;

    UPROPERTY() 
    UMySaveGame* PendingLoadedSave = nullptr;
    
    bool  bApplyLoadAfterTravel = false;
    int32 PendingSaveRetries = 0;
    int32 PendingLoadRetries = 0;
    static constexpr int32 MaxPendingSaveRetries = 120; // ~2s at 60fps
    static constexpr int32 MaxPendingLoadRetries = 120;

    FDelegateHandle PostLoadMapHandle;

    void HandlePostLoadMapWithWorld(UWorld* LoadedWorld);
    void TrySaveAfterSpawn();

    void TryApplyAfterTravel();                // waits for pawn then applies
    bool ApplyLoadedSave(UMySaveGame* LoadedSave);   // your current apply body, moved here

public:
    // Simple API - no slot names needed!
    
    UFUNCTION(BlueprintCallable, Category = "Saving")
    bool SaveGame();
    
    UFUNCTION(BlueprintCallable, Category = "Saving")
    bool LoadGame();
    
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Saving")
    bool DoesSaveExist() const;
    
    UFUNCTION(BlueprintCallable, Category = "Saving")
    bool DeleteSave();
    
    // Get last save info
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Saving")
    FDateTime GetLastSaveTime() const;

    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
};
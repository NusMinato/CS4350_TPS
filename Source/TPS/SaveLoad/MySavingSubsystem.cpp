// MySavingSubsystem.cpp
#include "MySavingSubsystem.h"
#include "Kismet/GameplayStatics.h"

void UMySavingSubsystem::HandlePostLoadMapWithWorld(UWorld* LoadedWorld)
{
    if (!LoadedWorld) return;

    // Handle load after travel first (allows loading from MainMenu)
    if (bApplyLoadAfterTravel && PendingLoadedSave)
    {
        PendingLoadRetries = 0;
        LoadedWorld->GetTimerManager().SetTimerForNextTick(
            FTimerDelegate::CreateUObject(this, &UMySavingSubsystem::TryApplyAfterTravel));
        return;
    }

    // Skip autosave for MainMenu (but allow loads from MainMenu to proceed above)
    if (LoadedWorld->GetMapName().Contains(TEXT("MainMenu"))) return;

    // Trigger save after loading a new map
    PendingSaveRetries = 0;
    LoadedWorld->GetTimerManager().SetTimerForNextTick(
        FTimerDelegate::CreateUObject(this, &UMySavingSubsystem::TrySaveAfterSpawn));
}

void UMySavingSubsystem::TrySaveAfterSpawn()
{
    UWorld* World = GetWorld();
    
    if (!World) return;

    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(World, 0);
    APlayerCharacter* PlayerCharacter = PlayerController ? Cast<APlayerCharacter>(PlayerController->GetPawn()) : nullptr;

    if (!PlayerCharacter || !PlayerCharacter->Inventory) {
        
        if (++PendingSaveRetries < this->MaxPendingSaveRetries) {
            World->GetTimerManager().SetTimerForNextTick(
                FTimerDelegate::CreateUObject(this, &UMySavingSubsystem::TrySaveAfterSpawn));
        }
        else {
            UE_LOG(LogTemp, Warning,
                TEXT("[SavingSubsystem] Player not ready after %d retries; skip entry autosave."),
                PendingSaveRetries);
        }

        return;
    }

    SaveGame();
}

void UMySavingSubsystem::TryApplyAfterTravel()
{
    UWorld* World = GetWorld();
    if (!World) return;

    APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
    APlayerCharacter* Player = PC ? Cast<APlayerCharacter>(PC->GetPawn()) : nullptr;

    if (Player && Player->Inventory && PendingLoadedSave)
    {
        if (ApplyLoadedSave(PendingLoadedSave))
        {
            PendingLoadedSave = nullptr;
            bApplyLoadAfterTravel = false;
            PendingLoadRetries = 0;

            // Schedule the save for next tick to ensure player is fully initialized
            // This guarantees the new level name is saved
            PendingSaveRetries = 0;
            World->GetTimerManager().SetTimerForNextTick(
                FTimerDelegate::CreateUObject(this, &UMySavingSubsystem::TrySaveAfterSpawn));
            return;
        }

        UE_LOG(LogTemp, Warning, TEXT("[SavingSubsystem] ApplyLoadedSave failed; will retry."));
    }

    // Retry if we haven't exceeded max retries
    if (++PendingLoadRetries < MaxPendingLoadRetries)
    {
        World->GetTimerManager().SetTimerForNextTick(
            FTimerDelegate::CreateUObject(this, &UMySavingSubsystem::TryApplyAfterTravel));
        return;
    }

    // Failed after all retries
    UE_LOG(LogTemp, Warning, TEXT("[SavingSubsystem] Player not ready after %d retries; skipping load apply."), PendingLoadRetries);
    PendingLoadedSave = nullptr;
    bApplyLoadAfterTravel = false;
    
    // Still try to save even if load failed (in case player exists but inventory wasn't ready)
    PendingSaveRetries = 0;
    World->GetTimerManager().SetTimerForNextTick(
        FTimerDelegate::CreateUObject(this, &UMySavingSubsystem::TrySaveAfterSpawn));
}

bool UMySavingSubsystem::ApplyLoadedSave(UMySaveGame* LoadedSave)
{
    APlayerCharacter* Player = Cast<APlayerCharacter>(
        UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

    if (!Player || !Player->Inventory)
    {
        UE_LOG(LogTemp, Error, TEXT("Player or inventory not found"));
        return false;
    }

    // Apply player stats
    Player->SetHealth(LoadedSave->CurrentHealth);
    Player->SetSanity(LoadedSave->CurrentSanity);
    Player->BP_OnStatusUpdated(Player->GetHealth(), Player->GetSanity());

    // Clear weapon slots and inventory
    Player->Inventory->Clear();
    Player->PrimaryWeaponItem = nullptr;
    Player->SecondaryWeaponItem = nullptr;
    Player->MeleeWeaponItem = nullptr;
    Player->ActiveWeaponItem = nullptr;
    Player->ActiveWeapon = nullptr;

    // Track loaded weapons for active weapon restoration
    TArray<UWeaponItem*> LoadedWeapons;

    // Load weapons
    for (const FWeaponSaveData& Data : LoadedSave->WeaponData)
    {
        UClass* WeaponClass = StaticLoadClass(
            UWeaponItem::StaticClass(), nullptr, *Data.WeaponClassName);

        if (!WeaponClass)
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to load weapon class: %s"), *Data.WeaponClassName);
            continue;
        }

        UWeaponItem* NewWeapon = NewObject<UWeaponItem>(Player->Inventory, WeaponClass);

        if (!NewWeapon)
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to create weapon: %s"), *Data.WeaponClassName);
            continue;
        }

        // Restore weapon data
        NewWeapon->CurrentAmmo = Data.CurrentAmmo;
        NewWeapon->MaxAmmo = Data.MaxAmmo;
        NewWeapon->Damage = Data.Damage;
        NewWeapon->SanityCost = Data.SanityCost;
        NewWeapon->WeaponType = static_cast<EMyWeaponType>(Data.WeaponType);
        NewWeapon->IsEquipped = Data.bIsEquipped;

        // Add to general inventory
        Player->Inventory->AddItem(NewWeapon);

        // If equipped, assign to appropriate weapon slot
        if (Data.bIsEquipped)
        {
            switch (NewWeapon->WeaponType)
            {
            case EMyWeaponType::LongGun:
                Player->PrimaryWeaponItem = NewWeapon;
                break;
            case EMyWeaponType::Pistol:
                Player->SecondaryWeaponItem = NewWeapon;
                break;
            case EMyWeaponType::ColdWeapon:
                Player->MeleeWeaponItem = NewWeapon;
                break;
            }
            UE_LOG(LogTemp, Log, TEXT("Weapon equipped to slot: %s"), *NewWeapon->GetClass()->GetName());
        }

        LoadedWeapons.Add(NewWeapon);

        UE_LOG(LogTemp, Log, TEXT("Loaded weapon: %s (Equipped: %s, Ammo: %d/%d)"),
            *Data.WeaponClassName,
            Data.bIsEquipped ? TEXT("Yes") : TEXT("No"),
            NewWeapon->CurrentAmmo, NewWeapon->MaxAmmo);
    }

    // Restore active weapon (the one held in hand)
    if (LoadedSave->ActiveWeaponIndex >= 0 &&
        LoadedSave->ActiveWeaponIndex < LoadedWeapons.Num())
    {
        UWeaponItem* ActiveWeapon = LoadedWeapons[LoadedSave->ActiveWeaponIndex];

        // SetActiveWeapon will spawn the RuntimeActor and attach to player
        ActiveWeapon->SpawnRuntimeActor();
        Player->SetActiveWeapon(ActiveWeapon);
        Player->BP_OnWeaponEquipped(ActiveWeapon);

        UE_LOG(LogTemp, Log, TEXT("Active weapon restored and attached: %s"),
            *ActiveWeapon->GetClass()->GetName());
    }

    // Load regular items (potions, consumables, etc.)
    for (const FItemSaveData& Data : LoadedSave->ItemData)
    {
        UClass* ItemClass = StaticLoadClass(
            UItem::StaticClass(), nullptr, *Data.ItemClassName);

        if (!ItemClass)
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to load item class: %s"), *Data.ItemClassName);
            continue;
        }

        // Create ONE item instance with the saved quantity
        UItem* NewItem = NewObject<UItem>(Player->Inventory, ItemClass);

        if (!NewItem)
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to create item: %s"), *Data.ItemClassName);
            continue;
        }

        // Set quantity from save data
        NewItem->Quantity = Data.Quantity;

        // Add to inventory (will stack if appropriate)
        Player->Inventory->AddItem(NewItem);

        UE_LOG(LogTemp, Log, TEXT("Loaded item: %s x%d"), *Data.ItemClassName, Data.Quantity);
    }

    UE_LOG(LogTemp, Log, TEXT("Game loaded successfully"));
    return true;
}

bool UMySavingSubsystem::SaveGame()
{
    FDateTime DateTime = FDateTime::Now();
    UE_LOG(LogTemp, Error, TEXT("Attempting to save game: %s"), *DateTime.ToString());

    UMySaveGame* SaveGame = Cast<UMySaveGame>(
        UGameplayStatics::CreateSaveGameObject(UMySaveGame::StaticClass()));
    
    if (!SaveGame)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create save game object"));
        return false;
    }
    
    // Get player
    APlayerCharacter* Player = Cast<APlayerCharacter>(
        UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
    
    if (!Player || !Player->Inventory)
    {
        UE_LOG(LogTemp, Error, TEXT("Player or inventory not found"));
        return false;
    }
    
    // Save metadata
    SaveGame->SaveTimestamp = FDateTime::Now();
    
    UWorld* World = GetWorld();
    if (World)
    {
        SaveGame->CurrentLevelName = UGameplayStatics::GetCurrentLevelName(World, /*bRemovePrefix=*/true);
        SaveGame->UnlockedLevels.Add(SaveGame->CurrentLevelName);
    }
    
    // Save player stats
    SaveGame->CurrentHealth = Player->GetHealth();
    SaveGame->CurrentSanity = Player->GetSanity();
    
    // Save weapons and items
    SaveGame->WeaponData.Empty();
    SaveGame->ItemData.Empty();
    
    for (UItem* Item : Player->Inventory->Items)
    {
        if (!Item) continue;
        
        // Handle weapons separately (each weapon is unique due to ammo)
        if (Item->IsA<UWeaponItem>())
        {
            UWeaponItem* Weapon = Cast<UWeaponItem>(Item);
            FWeaponSaveData Data;
            
            // If weapon has RuntimeActor, sync data from it
            if (Weapon->GetRuntimeActor())
            {
                Data.WeaponClassName = Weapon->GetClass()->GetPathName();
                Data.CurrentAmmo = Weapon->GetRuntimeActor()->CurrentAmmo;
                Data.MaxAmmo = Weapon->GetRuntimeActor()->MaxAmmo;
                Data.Damage = Weapon->GetRuntimeActor()->Damage;
                Data.SanityCost = Weapon->GetRuntimeActor()->SanityCost;
            }
            else
            {
                // Weapon not active, get data from item
                Data.WeaponClassName = Weapon->GetClass()->GetPathName();
                Data.CurrentAmmo = Weapon->CurrentAmmo;
                Data.MaxAmmo = Weapon->MaxAmmo;
                Data.Damage = Weapon->Damage;
                Data.SanityCost = Weapon->SanityCost;
            }
            
            Data.WeaponType = static_cast<uint8>(Weapon->WeaponType);
            Data.bIsEquipped = Weapon->IsEquipped;
            
            SaveGame->WeaponData.Add(Data);
            
            UE_LOG(LogTemp, Log, TEXT("Saved weapon: %s (Equipped: %s, Ammo: %d/%d)"), 
                *Weapon->GetClass()->GetName(), 
                Data.bIsEquipped ? TEXT("Yes") : TEXT("No"),
                Data.CurrentAmmo, Data.MaxAmmo);
        }
        else
        {
            // Save regular items with their quantity
            FItemSaveData Data;
            Data.ItemClassName = Item->GetClass()->GetPathName();
            Data.Quantity = Item->Quantity;  // Save the stack quantity directly
            
            SaveGame->ItemData.Add(Data);
            
            UE_LOG(LogTemp, Log, TEXT("Saved item: %s x%d"), 
                *Item->GetClass()->GetName(), Data.Quantity);
        }
    }
    
    // Save active weapon index
    SaveGame->ActiveWeaponIndex = -1;  // Default to no active weapon
    
    if (Player->ActiveWeaponItem)
    {
        // Find active weapon in the saved weapon data
        for (int32 i = 0; i < SaveGame->WeaponData.Num(); i++)
        {
            if (SaveGame->WeaponData[i].WeaponClassName == Player->ActiveWeaponItem->GetClass()->GetPathName())
            {
                SaveGame->ActiveWeaponIndex = i;
                UE_LOG(LogTemp, Log, TEXT("Active weapon index: %d"), SaveGame->ActiveWeaponIndex);
                break;
            }
        }
        
        // If not found, log warning
        if (SaveGame->ActiveWeaponIndex == -1)
        {
            UE_LOG(LogTemp, Warning, TEXT("Active weapon not found in weapon slots!"));
        }
    }
    
    // Save to disk
    bool bSuccess = UGameplayStatics::SaveGameToSlot(SaveGame, SaveSlotName, UserIndex);
    
    if (bSuccess)
    {
        DateTime = FDateTime::Now();
        UE_LOG(LogTemp, Log, TEXT("Game saved successfully: %s"), *DateTime.ToString());
    }
    else
    {
        DateTime = FDateTime::Now();
        UE_LOG(LogTemp, Error, TEXT("Failed to save game: %s"), *DateTime.ToString());
    }
    
    return bSuccess;
}

bool UMySavingSubsystem::LoadGame()
{
    FDateTime DateTime = FDateTime::Now();
    UE_LOG(LogTemp, Error, TEXT("Attempting to load game: %s"), *DateTime.ToString());

    if (!DoesSaveExist()) { UE_LOG(LogTemp, Warning, TEXT("No save")); return false; }

    UMySaveGame* Loaded = Cast<UMySaveGame>(
        UGameplayStatics::LoadGameFromSlot(SaveSlotName, UserIndex));
    if (!Loaded) { UE_LOG(LogTemp, Error, TEXT("Load failed")); return false; }

    if (Loaded->CurrentLevelName.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("Saved level name empty")); 
        return false;
    }

    // Always travel (even if the current map is the same) so we respawn at level entry
    PendingLoadedSave     = Loaded;
    bApplyLoadAfterTravel = true;

    DateTime = FDateTime::Now();
    UE_LOG(LogTemp, Error, TEXT("Attempting to open level: %s"), *DateTime.ToString());

    UGameplayStatics::OpenLevel(GetWorld(), FName(*Loaded->CurrentLevelName));
    return true; // important: return immediately; old world is tearing down
}

bool UMySavingSubsystem::DoesSaveExist() const
{
    return UGameplayStatics::DoesSaveGameExist(SaveSlotName, UserIndex);
}

bool UMySavingSubsystem::DeleteSave()
{
    if (!DoesSaveExist())
    {
        return false;
    }
    
    bool bSuccess = UGameplayStatics::DeleteGameInSlot(SaveSlotName, UserIndex);
    
    if (bSuccess)
    {
        UE_LOG(LogTemp, Log, TEXT("Save game deleted"));
    }
    
    return bSuccess;
}

void UMySavingSubsystem::OpenLevelWithCarryOver(FString LevelName)
{
    // 1) Snapshot current player into the normal save slot
    if (!SaveGame()) 
    {
        UE_LOG(LogTemp, Error, TEXT("[SavingSubsystem] Failed to save before level travel to %s"), *LevelName);
        return;
    }

    // 2) Keep it in memory and mark that we should apply after travel
    PendingLoadedSave = Cast<UMySaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, UserIndex));
    
    if (!PendingLoadedSave) 
    { 
        UE_LOG(LogTemp, Error, TEXT("[SavingSubsystem] Failed to load save game into memory")); 
        return; 
    }

    PendingLoadedSave->CurrentLevelName = LevelName;
    bApplyLoadAfterTravel = true;
    
    // 3) Travel
    UGameplayStatics::OpenLevel(GetWorld(), FName(*LevelName));
}

FDateTime UMySavingSubsystem::GetLastSaveTime() const
{
    if (!DoesSaveExist())
    {
        return FDateTime::MinValue();
    }
    
    UMySaveGame* LoadedSave = Cast<UMySaveGame>(
        UGameplayStatics::LoadGameFromSlot(SaveSlotName, UserIndex));
    
    if (LoadedSave)
    {
        return LoadedSave->SaveTimestamp;
    }
    
    return FDateTime::MinValue();
}

void UMySavingSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    PostLoadMapHandle = FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UMySavingSubsystem::HandlePostLoadMapWithWorld);
}

void UMySavingSubsystem::Deinitialize()
{
    if (PostLoadMapHandle.IsValid())
    {
        FCoreUObjectDelegates::PostLoadMapWithWorld.Remove(PostLoadMapHandle);
        PostLoadMapHandle.Reset();
    }
    Super::Deinitialize();
}

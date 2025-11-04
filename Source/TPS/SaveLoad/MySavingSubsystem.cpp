// MySavingSubsystem.cpp
#include "MySavingSubsystem.h"
#include "Kismet/GameplayStatics.h"

void UMySavingSubsystem::HandlePostLoadMapWithWorld(UWorld* LoadedWorld)
{
    if (LoadedWorld->GetMapName().Contains(TEXT("MainMenu"))) return;

    PendingSaveRetries = 0;

    if (LoadedWorld)
    {
        // Defer 1 tick to let GameMode/PlayerController/PlayerPawn spawn
        LoadedWorld->GetTimerManager().SetTimerForNextTick(
            FTimerDelegate::CreateUObject(this, &UMySavingSubsystem::TrySaveAfterSpawn));
    }
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

bool UMySavingSubsystem::SaveGame()
{
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
        SaveGame->CurrentLevelName = World->GetName();
        SaveGame->UnlockedLevels.Add(World->GetName());
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
        UE_LOG(LogTemp, Log, TEXT("Game saved successfully"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to save game"));
    }
    
    return bSuccess;
}

bool UMySavingSubsystem::LoadGame()
{
    if (!DoesSaveExist())
    {
        UE_LOG(LogTemp, Warning, TEXT("No save game exists"));
        return false;
    }
    
    UMySaveGame* LoadedSave = Cast<UMySaveGame>(
        UGameplayStatics::LoadGameFromSlot(SaveSlotName, UserIndex));
    
    if (!LoadedSave)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load save game"));
        return false;
    }

    if (LoadedSave->CurrentLevelName.IsEmpty()) {
        UE_LOG(LogTemp, Error, TEXT("Failed to read the game level"));
        return false;
    }
    
    UGameplayStatics::OpenLevel(GetWorld(), FName(*LoadedSave->CurrentLevelName));

    // Get player
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
        Player->SetActiveWeapon(ActiveWeapon);
        
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

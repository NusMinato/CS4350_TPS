// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerCharacter.h"
#include "Camera/PlayerCameraManager.h"
#include "../InventorySystem/ItemPickUpWrapper.h"
#include "../InventorySystem/Items/Interactable.h"
#include "../InventorySystem/Items/InventoryComponent.h"
#include "../InventorySystem/Items/Item.h"
#include "GameFramework/PlayerController.h"
#include "DrawDebugHelpers.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
	Inventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
	Inventory->Capacity = 20;
	CurrHealth = MaxHealth;
	CurrSanity = MaxSanity;
}

void APlayerCharacter::UseItem(UItem* Item)
{
	Item->Use(this);
	// blueprint event
	Item->OnUse(this);
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    // Assume "Interact" action mapping is bound to E in project settings:
    PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &APlayerCharacter::Interact);
}

void APlayerCharacter::Interact()
{
    // Perform a line trace from the camera
    FVector CameraLoc;
    FRotator CameraRot;
    APlayerController* PC = Cast<APlayerController>(GetController());
    if (PC)
    {
        PC->GetPlayerViewPoint(CameraLoc, CameraRot);  // get camera position and rotation
    }
    else
    {
        // Fallback: use actor eyes location if no PlayerController (e.g. AI)
        CameraLoc = GetActorLocation();
        CameraRot = GetActorRotation();
    }


    FVector TraceStart = CameraLoc;
    FVector TraceEnd = CameraLoc + (CameraRot.Vector() * InteractDistance);

    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);  // ignore self
    bool bHit = GetWorld()->LineTraceSingleByChannel(
        Hit, TraceStart, TraceEnd, ECC_Visibility, Params);

    // (Optional: Draw debug line to visualize the trace in development)
    DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Green, false, 1.0f);

    if (bHit && Hit.GetActor())
    {
        AActor* HitActor = Hit.GetActor();
        // Check if the hit actor implements our interactable interface
        if (HitActor->GetClass()->ImplementsInterface(UInteractable::StaticClass()))
        {
            // Call the interact function on the hit actor
            IInteractable::Execute_Interact(HitActor, this);
            // (This will trigger AItemPickUpWrapper::Interact_Implementation in C++, 
            // which calls OnPickUp to add to inventory and destroy the item.)
        }
    }
}
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "./Items/Interactable.h"
#include "Components/SphereComponent.h"
#include "ItemPickUpWrapper.generated.h"

class UItem;
class APlayerCharacter;

UCLASS()
class TPS_API AItemPickUpWrapper : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemPickUpWrapper();


	UPROPERTY(EditAnywhere, Instanced, BlueprintReadOnly, Category = "Item")
	UItem* WrappedItem = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	bool bAutoPickUp = false;



	UFUNCTION(BlueprintCallable)
	void OnPickUp(APlayerCharacter* PlayerCharacter);

protected:

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
		const FHitResult& SweepResult);	
	
	UPROPERTY(VisibleAnywhere, Category = "Item")
	USphereComponent* PickupCollision;


public:
	/** Interface implementation for interacting (picking up) */
	virtual void Interact_Implementation(APawn* InstigatorPawn) override;
	virtual FText GetInteractText_Implementation() const override;
};

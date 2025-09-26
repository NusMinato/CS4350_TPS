// Fill out your copyright notice in the Description page of Project Settings.


#include "SanityItem.h"
#include "InventoryComponent.h"
#include "../Player/PlayerCharacter.h"

void USanityItem::Use(APlayerCharacter* PlayerCharacter)
{
	int32 Sanity = PlayerCharacter->CurrSanity + this->RestorationAmount;
	PlayerCharacter->SetSanity(Sanity <= PlayerCharacter->MaxSanity ? Sanity : PlayerCharacter->MaxSanity);
}

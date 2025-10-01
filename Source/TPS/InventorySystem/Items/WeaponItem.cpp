// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponItem.h"
#include "../Player/PlayerCharacter.h"

void UWeaponItem::Use(APlayerCharacter* PC) {
	if (!PC) return;

	this->OnUse(PC);
}

void UWeaponItem::Relaod(APlayerCharacter* PC)
{
	int32 PlayerSanity = PC->GetSanity();
	if (PlayerSanity <= 0) {
		PC->SetSanity(0);
		return;
	}

	this->CurrentAmmo = this->MaxAmmo;
	PC->SetSanity(PlayerSanity - this->SanityCost);

}

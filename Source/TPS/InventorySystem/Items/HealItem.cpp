// Fill out your copyright notice in the Description page of Project Settings.

#include "HealItem.h"
#include "../Player/PlayerCharacter.h"

void UHealItem::Use(APlayerCharacter* PlayerCharacter) {
	int32 PlayerHealthAfterHeal = PlayerCharacter->CurrHealth + this->HealAmount;
	PlayerCharacter->CurrHealth = PlayerHealthAfterHeal > PlayerCharacter->MaxHealth ? PlayerCharacter->MaxHealth : PlayerHealthAfterHeal;
}
// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"

UItem::UItem() {
	this->ItemDisplayName = FText::FromString("Item");
	this->UseActionText = FText::FromString("Use");
}

void UItem::Use(APlayerCharacter* Character)
{
}

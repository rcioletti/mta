// Fill out your copyright notice in the Description page of Project Settings.

#include "MTA.h"
#include "MTACharacter.h"
#include "InventorySlotWidget.h"

void UInventorySlotWidget::SetEquippedItem()
{
	AMTACharacter* Char = Cast<AMTACharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (Char)
	{
		Char->SetEquippedItem(ItemTexture);
	}
}

void UInventorySlotWidget::SetItemTexture(AWeapon* Item)
{
	(Item) ? ItemTexture = Item->GetPickupTexture() : ItemTexture = nullptr;
}



// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "Weapon.h"
#include "InventorySlotWidget.generated.h"

/**
 * 
 */
UCLASS()
class MTA_API UInventorySlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UTexture2D* ItemTexture;

	UFUNCTION(BlueprintCallable, Category = UI)
	void SetEquippedItem();

public:
	UFUNCTION(BlueprintCallable, Category = UI)
	void SetItemTexture(AWeapon* Item);
	
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "Weapon.h"
#include "InventoryWidget.generated.h"

/**
 * 
 */
UCLASS()
class MTA_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, Category = UI)
	void Show();

	UFUNCTION(BlueprintImplementableEvent, Category = UI)
	void Hide();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<AWeapon*> ItemsArray;
};

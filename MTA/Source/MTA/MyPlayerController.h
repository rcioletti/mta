// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerController.h"
#include "InventoryWidget.h"
#include "MyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class MTA_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()
	
	
private:
	UInventoryWidget* InventoryWidgetRef;

	bool bIsInventoryOpen;

protected:
	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<UInventoryWidget> InventoryWidgetBP;


public:
	virtual void Possess(APawn* InPawn) override;

	void HandleInventoryInput();
	
	
};

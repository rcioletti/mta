// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Character.h"
#include "Weapon.h"
#include "InteractionInterface.h"
#include "MTACharacter.generated.h"

#define MAX_INVENTORY_ITEMS 4

UENUM()
enum class ESoldierTaskEnum : uint8
{
	None,
	FullBody,
	Interact,
};

UCLASS(config=Game)
class AMTACharacter : public ACharacter
{
	GENERATED_BODY()

	virtual void Tick(float DeltaSeconds) override;

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(EditDefaultsOnly, Category = Inventory)
	UStaticMeshComponent * BackpackComponent;

	UPROPERTY(EditDefaultsOnly, Category = Inventory)
	UStaticMesh * Backpack;

	UPROPERTY(EditDefaultsOnly, Category = Inventory)
	class USoundBase* PickupWeaponSound;

	UPROPERTY(EditDefaultsOnly, Category = Inventory)
	class USoundBase* EquipWeaponSound;

	UFUNCTION()
	void FireWeapon();

	UPROPERTY(VisibleAnywhere, Category = Spawn)
	TSubclassOf<class AWeapon> WeaponSpawn;

	AWeapon *CurrentWeapon;

	UFUNCTION()
	virtual void BeginPlay() override;

protected:

	UPROPERTY(EditAnywhere)
	float RaycastRange = 250.f;

	

public:
	AMTACharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Inventory)
	TArray<class AWeapon*> Inventory;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	void OnSuicide();

	void Equip();

	void UnEquip();

	void StartAiming();

	void StopAiming();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation")
	bool bAiming;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation")
	bool bIsEquipped;


	/**Health of my character*/
	UPROPERTY(ReplicatedUsing = OnRep_Health)
		float Health;

	UPROPERTY(EditAnywhere, Category = Stats)
		float MaxHealth = 100.f;

	UFUNCTION()
		void OnRep_Health();

	UFUNCTION()
		void HandleInventoryInput();

protected:

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);





protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	// End of APawn interface

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

private:
	virtual void LifeSpanExpired();

private:
	AWeapon* CurrentlyEquippedItem;

public:
	void SetEquippedItem(UTexture2D* Texture);

	UFUNCTION()
	virtual void OnOverlapBegin(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	AWeapon* LastItemSeen;


};


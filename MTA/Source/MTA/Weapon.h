// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

#define TRACE_WEAPON ECC_GameTraceChannel1

UENUM(BlueprintType)
namespace EWeaponProjectile
{
	enum ProjectileType
	{
		EBullet			UMETA(DisplayName = "Bullet"),
		ESpread			UMETA(DisplayName = "Spread"),
		EProjectile		UMETA(DisplayName = "Projectile"),
	};
}

USTRUCT()
struct FWeaponData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, Category = Ammo)
	int32 MaxAmmo;

	UPROPERTY(EditDefaultsOnly, Category = Ammo)
	int32 ShotCost;

	UPROPERTY(EditDefaultsOnly, Category = Config)
	float TimeBetweenShots;

	UPROPERTY(EditDefaultsOnly, Category = Config)
	float WeaponRange;

	UPROPERTY(EditDefaultsOnly, Category = Config)
	float WeaponSpread;

	
};


UCLASS()
class MTA_API AWeapon : public AActor
{
	GENERATED_BODY()
	
		virtual void BeginPlay() override;

	UFUNCTION()
		void OnTriggerEnter(AActor* OverlapedActor, AActor* OtherActor);

	UFUNCTION()
		void OnTriggerExit(AActor* OverlapedActor, AActor* OtherActor);
public:
	// Sets default values for this actor's properties
	AWeapon();

	UFUNCTION()
		void Fire();

	UFUNCTION()
		void Instant_Fire();

	UPROPERTY(EditDefaultsOnly, Category = Config)
		FWeaponData WeaponConfig;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Config)
		TEnumAsByte<EWeaponProjectile::ProjectileType> ProjectileType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Collision)
	class USphereComponent* CollisionComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Config)
	class USkeletalMeshComponent* WeaponMesh;


protected:

	FHitResult WeaponTrace(const FVector &TraceFrom, const FVector &TraceTo) const;

	void ProcessInstantHit(const FHitResult &Impact, const FVector &Origin, const FVector &ShootDir, int32 RandomSeed, float ReticleSpread);

public:

	void UpdateWeaponPhysics();
	
	UPROPERTY(EditDefaultsOnly, Category = Config)
	class USoundBase* FireShoot;
	
	UPROPERTY(EditDefaultsOnly, Category = Config)
	class UParticleSystem* MuzzleFlash;

	UPROPERTY(EditDefaultsOnly, Category = Config)
	class UParticleSystem* SmokeChamber;
	
	UPROPERTY(EditDefaultsOnly, Category = Config)
	class UParticleSystem* ImpactParticle;

	UPROPERTY(EditDefaultsOnly, Category = Config)
	class UMaterialInterface* BulletHole;

	bool bCanEquip;

	void SetGlowEffect(bool Status);

	FORCEINLINE UTexture2D* GetPickupTexture() { return PickupTexture; }

protected:
	
	UPROPERTY(EditAnywhere, Category = "PickupProperties")
	UTexture2D* PickupTexture;

	UPROPERTY(EditAnywhere, Category = "PickupProperties")
	FString ItemName;

};

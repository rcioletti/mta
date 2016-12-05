// Fill out your copyright notice in the Description page of Project Settings.

#include "MTA.h"
#include "Weapon.h"
#include "Engine.h"


// Sets default values
AWeapon::AWeapon()
{

	CollisionComp = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionComp"));
	RootComponent = CollisionComp;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered;
	WeaponMesh->bReceivesDecals = false;
	WeaponMesh->CastShadow = true;
	WeaponMesh->SetCollisionProfileName(FName(TEXT("PhysicsActor")));
	WeaponMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	WeaponMesh->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	WeaponMesh->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	WeaponMesh->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
	WeaponMesh->bEnablePhysicsOnDedicatedServer = true;
	WeaponMesh->SetSimulatePhysics(true);
	WeaponMesh->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
	WeaponMesh->SetupAttachment(RootComponent);

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PrePhysics;
	SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
	bReplicates = true;
	bNetUseOwnerRelevancy = true;

}

void AWeapon::Fire() 
{
	if (ProjectileType == EWeaponProjectile::EBullet){
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Black, TEXT("Bullet"));
		Instant_Fire();
	}
	if (ProjectileType == EWeaponProjectile::ESpread) {
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Black, TEXT("Spread"));
		for (int32 i = 0; i <= WeaponConfig.WeaponSpread; i++) {
			Instant_Fire();
		}
	}
	if (ProjectileType == EWeaponProjectile::EProjectile) {
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Black, TEXT("Projectile"));
	}
}

void AWeapon::Instant_Fire() 
{
	const int32 RandomSeed = FMath::Rand();
	FRandomStream WeaponRandomStream(RandomSeed);
	const float CurrentSpread = WeaponConfig.WeaponSpread;
	const float SpreadCone = FMath::DegreesToRadians(WeaponConfig.WeaponSpread * 0.5f);
	const FVector AimDir = WeaponMesh->GetSocketRotation("MF").Vector();
	const FVector StartTrace = WeaponMesh->GetSocketLocation("MF");
	const FVector ShootDir = WeaponRandomStream.VRandCone(AimDir, SpreadCone, SpreadCone);
	const FVector EndTrace = StartTrace + ShootDir * WeaponConfig.WeaponRange;
	const FHitResult Impact = WeaponTrace(StartTrace, EndTrace);

	ProcessInstantHit(Impact, StartTrace, ShootDir, RandomSeed, CurrentSpread);
}

FHitResult AWeapon::WeaponTrace(const FVector &TraceFrom, const FVector &TraceTo) const
{
	static FName WeaponFireTag = FName(TEXT("WeaponTrace"));

	FCollisionQueryParams TraceParams(WeaponFireTag, true, Instigator);
	TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = true;
	TraceParams.AddIgnoredActor(this);

	FHitResult Hit(ForceInit);

	GetWorld()->LineTraceSingleByChannel(Hit, TraceFrom, TraceTo, TRACE_WEAPON, TraceParams);
	
	return Hit;

}

void AWeapon::ProcessInstantHit(const FHitResult &Impact, const FVector &Origin, const FVector &ShootDir, int32 RandomSeed, float ReticleSpread) 
{
	const FVector EndTrace = Origin + ShootDir * WeaponConfig.WeaponRange;
	const FVector EndPoint = Impact.GetActor() ? Impact.ImpactPoint : EndTrace;
	DrawDebugLine(this->GetWorld(), Origin, Impact.TraceEnd, FColor::Red, true, 10000, 10.0f);
	const FRotator AimDir = WeaponMesh->GetSocketRotation("MF");

	if (FireShoot != NULL) {
		UGameplayStatics::PlaySoundAtLocation(this, FireShoot, GetActorLocation());
	}
	if (MuzzleFlash != NULL) {
		UGameplayStatics::SpawnEmitterAttached(MuzzleFlash, WeaponMesh, ("MF"));
	}
	if (SmokeChamber != NULL) {
		UGameplayStatics::SpawnEmitterAttached(SmokeChamber, WeaponMesh, ("MF"));
	}
	if (BulletHole != NULL) {
		UGameplayStatics::SpawnDecalAtLocation(this, BulletHole, FVector(5.f, 5.f, 5.f),Impact.ImpactPoint, AimDir, 20.f);
	}
	if (ImpactParticle != NULL) {
		UGameplayStatics::SpawnEmitterAtLocation(this, ImpactParticle, Impact.ImpactPoint, GetActorRotation());
	}

}

void AWeapon::UpdateWeaponPhysics() {

}
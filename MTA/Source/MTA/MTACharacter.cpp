// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "MTA.h"
#include "MTACharacter.h"
#include "UnrealNetwork.h"
#include "Engine.h"

//////////////////////////////////////////////////////////////////////////
// AMTACharacter

#define DefaultHealth 100.f

AMTACharacter::AMTACharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	Health = DefaultHealth;

	static ConstructorHelpers::FObjectFinder<UBlueprint> WeaponBlueprint(TEXT("Blueprint'/Game/BluePrints/Ak47.Ak47'"));

	WeaponSpawn = NULL;
	
	if (WeaponBlueprint.Succeeded())
	{
		WeaponSpawn = (UClass*)WeaponBlueprint.Object->GeneratedClass;
	}

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)

	GetMesh()->SetCollisionProfileName(FName(TEXT("Ragdoll")));
	GetMesh()->SetCollisionResponseToAllChannels(ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);
	GetMesh()->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::AlwaysTickPoseAndRefreshBones;

	FPostProcessSettings & PPS = FollowCamera->PostProcessSettings;
	PPS.bOverride_FilmSaturation = true;
	PPS.bOverride_SceneFringeIntensity = true;
	PPS.bOverride_VignetteIntensity = true;
}

//////////////////////////////////////////////////////////////////////////
// Input


void AMTACharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	// Set up gameplay key bindings
	check(InputComponent);
	InputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	InputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	InputComponent->BindAction("Fire", IE_Pressed, this, &AMTACharacter::FireWeapon);

	InputComponent->BindAxis("MoveForward", this, &AMTACharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AMTACharacter::MoveRight);


	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	InputComponent->BindAxis("TurnRate", this, &AMTACharacter::TurnAtRate);
	InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	InputComponent->BindAxis("LookUpRate", this, &AMTACharacter::LookUpAtRate);

	// handle touch devices
	InputComponent->BindTouch(IE_Pressed, this, &AMTACharacter::TouchStarted);
	InputComponent->BindTouch(IE_Released, this, &AMTACharacter::TouchStopped);

	InputComponent->BindAction("Suicide", IE_Pressed, this, &AMTACharacter::OnSuicide);

}

void AMTACharacter::BeginPlay() 
{
	Super::BeginPlay();


		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = Instigator;
		AWeapon *Spawner = GetWorld()->SpawnActor<AWeapon>(WeaponSpawn, SpawnParams);

		if (Spawner) {
			Spawner->AttachRootComponentTo(GetMesh(), "GunSocket", EAttachLocation::SnapToTarget);
			CurrentWeapon = Spawner;
		}

}


void AMTACharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	// jump, but only on the first touch
	if (FingerIndex == ETouchIndex::Touch1)
	{
		Jump();
	}
}

void AMTACharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	if (FingerIndex == ETouchIndex::Touch1)
	{
		StopJumping();
	}
}

void AMTACharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMTACharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AMTACharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AMTACharacter::MoveRight(float Value)
{
	if ( (Controller != NULL) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

//////////////////////////////////////////////////////////
////////////////////////MTAPART//////////////////////////

void AMTACharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime); // Call parent class tick function 

	if (GetNetMode() != NM_DedicatedServer)
	{
		OnRep_Health();
	}
}

void AMTACharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMTACharacter, Health);
}

void AMTACharacter::OnSuicide() {

	Health = 0.f;
}


void AMTACharacter::OnRep_Health()
{
	if (Health <= 0.f)
	{
		// Activate ragdoll if health <= 0
		GetMesh()->SetAllBodiesSimulatePhysics(true);

		LifeSpanExpired();
	
	}

	// Set saturation
	FPostProcessSettings & PPS = FollowCamera->PostProcessSettings;
	PPS.FilmSaturation = Health / DefaultHealth;
	PPS.SceneFringeIntensity = FMath::GetMappedRangeValueClamped(FVector2D(0.f, 20.f), FVector2D(5.f, 0.f), Health);
	PPS.VignetteIntensity = FMath::GetMappedRangeValueClamped(FVector2D(0.f, 50.f), FVector2D(1.f, 0.f), Health);
}

void AMTACharacter::FireWeapon() {

	CurrentWeapon->Fire();
}

void AMTACharacter::LifeSpanExpired() 
{
	APlayerController* playerController = Cast<APlayerController>(GetController());

	Super::LifeSpanExpired();

	if (playerController)
	{
		GetWorld()->GetAuthGameMode()->RestartPlayer(playerController);
	}
}

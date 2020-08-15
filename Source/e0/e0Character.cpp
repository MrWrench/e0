// Copyright Epic Games, Inc. All Rights Reserved.

#include "e0Character.h"
#include "e0Projectile.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "MotionControllerComponent.h"
#include "E0BaseWeapon.h"
#include "E0BaseGadget.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// Ae0Character

Ae0Character::Ae0Character()
{
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	ViewKick = FVector::ZeroVector;
	RecoilRecoverySpeed = 20;
}

void Ae0Character::BeginPlay()
{
	Super::BeginPlay();
}

//////////////////////////////////////////////////////////////////////////
// Input

void Ae0Character::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &Ae0Character::StartPrimaryFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &Ae0Character::StopPrimaryFire);
	
	PlayerInputComponent->BindAction("UseGadget", IE_Pressed, this, &Ae0Character::StartUsingGadget);
	PlayerInputComponent->BindAction("UseGadget", IE_Released, this, &Ae0Character::StopUsingGadget);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &Ae0Character::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &Ae0Character::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &Ae0Character::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &Ae0Character::LookUpAtRate);
}

void Ae0Character::GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const
{
	OutLocation = FirstPersonCameraComponent->GetComponentLocation();
	OutRotation = FirstPersonCameraComponent->GetComponentRotation();
}

void Ae0Character::StartPrimaryFire()
{
	if(!CurrentWeapon)
		return;

	CurrentWeapon->StartPrimaryFire();
	OnStartPrimaryFire();
}

void Ae0Character::StopPrimaryFire()
{
	if(!CurrentWeapon)
		return;
	
	CurrentWeapon->StopPrimaryFire();
	OnStopPrimaryFire();
}

void Ae0Character::StartUsingGadget()
{
	if(!EquippedGadget)
		return;

	EquippedGadget->StartUsing();
	OnStartUsingGadget();
}

void Ae0Character::StopUsingGadget()
{
	if(!EquippedGadget)
		return;
	
	EquippedGadget->StopUsing();
	OnStopUsingGadget();
}

void Ae0Character::AddViewKick(FVector KickVector)
{
	ViewKick += KickVector;
	AddControllerPitchInput(-KickVector.X);
	AddControllerYawInput(KickVector.Y);
	AddControllerRollInput(KickVector.Z);
}

void Ae0Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	const FVector NewViewKick = FMath::VInterpTo(ViewKick, FVector::ZeroVector, DeltaTime, RecoilRecoverySpeed);
	const FVector ViewKickDelta = ViewKick - NewViewKick;
	ViewKick = NewViewKick;
	AddControllerPitchInput(ViewKickDelta.X);
	AddControllerYawInput(ViewKickDelta.Y);
	AddControllerRollInput(ViewKickDelta.Z);
}

void Ae0Character::EquipWeapon(AE0BaseWeapon* NewWeapon)
{
	if(!NewWeapon)
		return;
	
	Weapons.Add(NewWeapon);
	NewWeapon->SetOwner(this);
	NewWeapon->Equip();
	const FAttachmentTransformRules AttachmentTransformRules(EAttachmentRule::SnapToTarget, false);
	NewWeapon->AttachToComponent(Mesh1P, AttachmentTransformRules, TEXT("GripPoint"));
	OnEquipWeapon(NewWeapon);

	SwitchWeapon(NewWeapon);
}

void Ae0Character::EquipGadget(AE0BaseGadget* NewGadget)
{
	if(!NewGadget)
		return;
	
	NewGadget->SetOwner(this);
	NewGadget->Equip();
	const FAttachmentTransformRules AttachmentTransformRules(EAttachmentRule::SnapToTarget, false);
	NewGadget->AttachToComponent(FirstPersonCameraComponent, AttachmentTransformRules);
	EquippedGadget = NewGadget;
}

void Ae0Character::SwitchWeapon(AE0BaseWeapon* NewWeapon)
{
	if(!NewWeapon)
		return;
	CurrentWeapon = NewWeapon;
	OnSwitchWeapon(NewWeapon);
}

void Ae0Character::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void Ae0Character::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void Ae0Character::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void Ae0Character::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}
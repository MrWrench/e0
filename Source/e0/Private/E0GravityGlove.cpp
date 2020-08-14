// Fill out your copyright notice in the Description page of Project Settings.


#include "E0GravityGlove.h"

#include "Camera/CameraComponent.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "GameFramework/DamageType.h"

AE0GravityGlove::AE0GravityGlove()
{
    PrimaryActorTick.bCanEverTick = true;

    ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
    ArrowComponent->SetHiddenInGame(true);
    RootComponent = ArrowComponent;

    PhysicsHandleComponent = CreateDefaultSubobject<UPhysicsHandleComponent>(TEXT("PhysicsHandleComponent"));

    PushCooldown = 0.3f;
    PullCooldown = 0.7f;
    MaxRange = 1400;
    MaxGrabRange = 700;
    IsGrabbing = false;
    NextPushTime = 0;
    MaxPullMass = 100;
    MaxPushMass = 250;
    Error = 0;
    ErrorTime = 0;
    GrabbedComponentMaxSpeed = 1000;
    Error = 44;
    InitialGrabErrorTime = -1;
    GrabDistanceBBoxMultiplier = 2.5f;
    MinGrabDistance = 300;
    MinGrabAngle = -60;
    MaxGrabAngle = 60;
    GrabDelay = 0.4f;
}

void AE0GravityGlove::PushComponent(UPrimitiveComponent* PushedComponent, const FVector ForwardDirection, const FName BoneName,
                                    const float Distance)
{
    if (PushedComponent == nullptr || !PushedComponent->IsSimulatingPhysics())
        return;

    PushedComponent->AddImpulse(GetPushImpulse(PushedComponent->GetMass(), ForwardDirection, Distance), BoneName);
}

void AE0GravityGlove::PushComponentAtLocation(UPrimitiveComponent* PushedComponent, const FVector ForwardDirection, FVector Location,
                                              const FName BoneName, const float Distance)
{
    if (PushedComponent == nullptr || !PushedComponent->IsSimulatingPhysics())
        return;

    PushedComponent->AddImpulseAtLocation(GetPushImpulse(PushedComponent->GetMass(), ForwardDirection, Distance), Location, BoneName);
}

FVector AE0GravityGlove::GetPushImpulse(const float Mass, const FVector Direction, const float Distance) const
{
    const float PushedMass = FMath::Min(MaxPushMass, Mass);
    return Direction * (PushVelocity->GetFloatValue(Distance) * PushedMass);
}

void AE0GravityGlove::GrabComponent(UPrimitiveComponent* GrabbedComponent, FName BoneName)
{
    if (GrabbedComponent == nullptr || !GrabbedComponent->IsSimulatingPhysics() || GrabbedComponent->GetMass() > MaxPullMass)
        return;

    GrabbedComponentMass = GrabbedComponent->GetMass();
    GrabbedComponent->SetMassOverrideInKg(BoneName, 1, true);
    const FTransform ComponentTransform = GrabbedComponent->GetComponentTransform();
    PhysicsHandleComponent->GrabComponentAtLocationWithRotation(GrabbedComponent, BoneName, ComponentTransform.GetLocation(),
                                                                ComponentTransform.Rotator());
    CurrentGrabDistance = FMath::Max(GrabbedComponent->Bounds.SphereRadius * GrabDistanceBBoxMultiplier, MinGrabDistance);
    CurrentGrabRotation = GetActorTransform().InverseTransformRotation(ComponentTransform.Rotator().Quaternion()).Rotator();

    IsGrabbing = true;
    ErrorTime = InitialGrabErrorTime;
    Error = 0;
    OnGrabComponent(GrabbedComponent, BoneName);
}

void AE0GravityGlove::ReleaseGrabbedComponent()
{
    const auto GrabbedComp = PhysicsHandleComponent->GetGrabbedComponent();
    const FName GrabbedBoneName = PhysicsHandleComponent->GrabbedBoneName;
    if (!IsGrabbing || GrabbedComp == nullptr)
        return;

    PhysicsHandleComponent->ReleaseComponent();
    GrabbedComp->SetMassOverrideInKg(GrabbedBoneName, GrabbedComponentMass);
    IsGrabbing = false;
    CurrentGrabDistance = MinGrabDistance;

    const FVector LinearVelocity = OwnerCharacter ? OwnerCharacter->GetVelocity() : FVector::ZeroVector;
    GrabbedComp->SetPhysicsLinearVelocity(LinearVelocity);
    GrabbedComp->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
    OnReleaseGrabbedComponent(GrabbedComp);
}

void AE0GravityGlove::PullComponent(UPrimitiveComponent* PulledComponent, FVector ForwardDirection, FName BoneName, float Distance)
{
    if (!PulledComponent || !PulledComponent->IsSimulatingPhysics() || PulledComponent->GetMass() > MaxPullMass)
        return;

    const float PulledMass = FMath::Min(MaxPushMass, PulledComponent->GetMass());
    const FVector Force = -ForwardDirection * (PullAcceleration->GetFloatValue(Distance) * PulledMass);
    PulledComponent->AddForce(Force, BoneName);
}

void AE0GravityGlove::DoGrabJump()
{
    if (OwnerCharacter == nullptr)
        return;

    UCharacterMovementComponent* CharacterMovementComponent = OwnerCharacter->GetCharacterMovement();
    if (CharacterMovementComponent == nullptr || !CharacterMovementComponent->IsFalling())
        return;

    const FVector LaunchVelocity = OwnerCharacter->GetActorUpVector() * GrabJumpVelocity;
    OwnerCharacter->LaunchCharacter(LaunchVelocity, false, true);
}

void AE0GravityGlove::DoSelfPush(const float Distance)
{
    if (OwnerCharacter == nullptr)
        return;

    UCharacterMovementComponent* CharacterMovementComponent = OwnerCharacter->GetCharacterMovement();
    if (CharacterMovementComponent == nullptr || !CharacterMovementComponent->IsFalling())
        return;

    const FVector PlayerMovementVector = CharacterMovementComponent->GetPendingInputVector();
    const FVector ActorForwardVector = GetActorForwardVector();
    const FVector LaunchDirection = -ActorForwardVector + PlayerMovementVector * FMath::Min(
        FVector::DotProduct(ActorForwardVector, -GetActorUpVector()), 0.0f);
    const FVector LaunchVelocity = LaunchDirection * SelfPushVelocity->GetFloatValue(Distance);
    OwnerCharacter->LaunchCharacter(LaunchVelocity, false, false);
}

void AE0GravityGlove::DoSelfPull()
{
    if (OwnerCharacter == nullptr)
        return;

    UCharacterMovementComponent* CharacterMovementComponent = OwnerCharacter->GetCharacterMovement();
    if (CharacterMovementComponent == nullptr || !CharacterMovementComponent->IsFalling())
        return;

    const FVector Acceleration = GetActorForwardVector() * SelfPullAcceleration;
    CharacterMovementComponent->AddForce(Acceleration);
}

float AE0GravityGlove::ComputeError()
{
    if (ErrorTime <= 0)
        return 0;

    // This code is ripped right from Half-Life 2
    ErrorTime = FMath::Min(ErrorTime, 1.0f);

    const UPrimitiveComponent* GrabbedComponent = PhysicsHandleComponent->GetGrabbedComponent();
    const FVector TargetLocation = GetGrabTargetLocation();
    const FVector GrabbedComponentLocation = GrabbedComponent->GetComponentLocation();
    float NewError = (GrabbedComponentLocation - TargetLocation).Size();
    const float NewErrorSpeed = NewError / ErrorTime;
    if (NewErrorSpeed > GrabbedComponentMaxSpeed)
        NewError *= 0.5f;
    Error = (1 - ErrorTime) * Error + NewError * ErrorTime;

    ErrorTime = 0;
    return Error;
}

void AE0GravityGlove::StartUsing()
{
    Super::StartUsing();
    NextGrabTime = GetWorld()->TimeSeconds + GrabDelay;
}

void AE0GravityGlove::StopUsing()
{
    if (!UseInput)
        return;

    Super::StopUsing();
    if (GetWorld()->TimeSeconds < NextPushTime)
        return;
    NextPushTime = GetWorld()->TimeSeconds + PushCooldown;
    NextPullTime = GetWorld()->TimeSeconds + PullCooldown;
    if (IsGrabbing)
    {
        UPrimitiveComponent* GrabbedComp = PhysicsHandleComponent->GetGrabbedComponent();
        const FName GrabbedBone = PhysicsHandleComponent->GrabbedBoneName;
        if (GrabbedComp != nullptr)
        {
            ReleaseGrabbedComponent();
            PushComponent(GrabbedComp, GetActorForwardVector(), GrabbedBone, 0);
            DoSelfPush(0);
        }
    }
    else
    {
        FHitResult Hit;
        const FVector Start = GetActorLocation();
        const FVector End = Start + GetActorForwardVector() * MaxRange;
        FCollisionQueryParams CollisionParams;
        if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, CollisionParams))
        {
            UPrimitiveComponent* HitComponent = Hit.GetComponent();
            if (HitComponent != nullptr && HitComponent->IsSimulatingPhysics(Hit.BoneName))
                PushComponentAtLocation(HitComponent, GetActorForwardVector(), Hit.Location, Hit.BoneName, Hit.Distance);
            DoSelfPush(Hit.Distance);
        }
    }
}

FVector AE0GravityGlove::GetGrabTargetLocation() const
{
    FRotator Rotator = GetActorRotation();
    Rotator.Pitch = FMath::ClampAngle(Rotator.Pitch, MinGrabAngle, MaxGrabAngle);
    return GetActorLocation() + Rotator.Vector() * CurrentGrabDistance;
}

bool AE0GravityGlove::IsPulling() const
{
    return UseInput && NextPullTime <= GetWorld()->TimeSeconds;
}

void AE0GravityGlove::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    AActor* MyOwner = GetOwner();
    if(!MyOwner)
        return;

    if (!IsPulling())
        return;

    if (IsGrabbing)
    {
        ErrorTime += DeltaTime;
        if (ComputeError() > MaxError)
        {
            ReleaseGrabbedComponent();
            UseInput = false;
        }
        else
        {
            FRotator NewRotation = GetActorTransform().TransformRotation(CurrentGrabRotation.Quaternion()).Rotator();
            PhysicsHandleComponent->SetTargetLocationAndRotation(GetGrabTargetLocation(), NewRotation);
        }
    }
    else
    {
        FHitResult Hit;
        const FVector Start = GetActorLocation();
        const FVector End = Start + GetActorForwardVector() * MaxRange;
        FCollisionQueryParams CollisionParams;
        CollisionParams.AddIgnoredActor(this);
        CollisionParams.AddIgnoredActor(MyOwner);
        
        if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, CollisionParams))
        {
            UPrimitiveComponent* HitComponent = Hit.GetComponent();
            if (HitComponent != nullptr && HitComponent->IsSimulatingPhysics(Hit.BoneName) && HitComponent->GetMass() <= MaxPullMass)
            {
                if (Hit.Distance > MaxGrabRange || GetWorld()->GetTimeSeconds() < NextGrabTime)
                {
                    if((GetActorLocation() - HitComponent->GetComponentLocation()).Size() > MinGrabDistance)
                    {
                        PullComponent(HitComponent, GetActorForwardVector(), Hit.BoneName, Hit.Distance);
                        DoSelfPull();
                    }
                }
                else
                {
                    GrabComponent(HitComponent, Hit.BoneName);
                    DoGrabJump();
                }
            }
        }
    }
}

bool AE0GravityGlove::OnCharacterGetPointDamage_Implementation(float Damage, const UDamageType* DamageType, FVector HitLocation,
    FVector HitNormal, UPrimitiveComponent* HitComponent, FName BoneName, FVector ShotFromDirection, AController* InstigatedBy,
    AActor* DamageCauser, const FHitResult& HitInfo)
{
    const bool IsCatchedBullet = IsPulling() && !IsGrabbing && FVector::DotProduct(GetActorForwardVector(), ShotFromDirection);    
    return IsCatchedBullet;
}

void AE0GravityGlove::BeginPlay()
{
    Super::BeginPlay();
    OwnerCharacter = Cast<ACharacter>(GetOwner());
}

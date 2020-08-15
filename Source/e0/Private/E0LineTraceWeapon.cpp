// Fill out your copyright notice in the Description page of Project Settings.


#include "E0LineTraceWeapon.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"

static TAutoConsoleVariable<int32> CVarDebugWeapons(TEXT("E0.DebugWeapons"), 0, TEXT("Draw Debug Lines for Weapons"), ECVF_Cheat);

AE0LineTraceWeapon::AE0LineTraceWeapon()
{
    MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
    RootComponent = MeshComp;

    ShootRange = 10000.0f;
    Damage = 20;
    ImpulseStrength = 5000;
}

void AE0LineTraceWeapon::Fire()
{
    FireBullet();
    OnFire();
}

void AE0LineTraceWeapon::FireBullet()
{
    AActor* MyOwner = GetOwner();
    if (!MyOwner)
        return;

    FVector EyeLocation;
    FRotator EyeRotation;
    MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);
    auto ShootDirection = EyeRotation.Vector();
    const FVector TraceEnd = EyeLocation + (ShootDirection * ShootRange);

    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(MyOwner);
    QueryParams.AddIgnoredActor(this);

    if(CVarDebugWeapons.GetValueOnGameThread() != 0)
        DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::Red, false, 1.0f, 0, 0.5f);

    FHitResult HitResult;
    if (!GetWorld()->LineTraceSingleByChannel(HitResult, EyeLocation, TraceEnd, ECC_Visibility, QueryParams))
        return;

    AActor* HitActor = HitResult.GetActor();
    UGameplayStatics::ApplyPointDamage(HitActor, Damage, ShootDirection, HitResult, MyOwner->GetInstigatorController(), this, DamageType);

    auto PrimitiveComponent = HitResult.GetComponent();
    if (PrimitiveComponent && PrimitiveComponent->IsSimulatingPhysics())
    {
        FVector Impulse = ShootDirection * ImpulseStrength;
        PrimitiveComponent->AddImpulseAtLocation(Impulse, HitResult.Location, HitResult.BoneName);
    }
}

void AE0LineTraceWeapon::StartPrimaryFire()
{
    Fire();
}

void AE0LineTraceWeapon::Equip()
{
    Super::Equip();
    MeshComp->SetCastShadow(false);
    SetActorEnableCollision(false);
}

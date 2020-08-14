// Fill out your copyright notice in the Description page of Project Settings.


#include "E0LineTraceWeapon.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"

AE0LineTraceWeapon::AE0LineTraceWeapon()
{
    MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
    RootComponent = MeshComp;

    ShootRange = 10000.0f;
    Damage = 20;
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

    DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::Red, false, 1.0f, 0, 0.5f);

    FHitResult HitResult;
    if(!GetWorld()->LineTraceSingleByChannel(HitResult, EyeLocation, TraceEnd, ECC_Visibility, QueryParams))
        return;

    AActor* HitActor = HitResult.GetActor();
    UGameplayStatics::ApplyPointDamage(HitActor, Damage, ShootDirection, HitResult, MyOwner->GetInstigatorController(), this, DamageType);
}

void AE0LineTraceWeapon::StartPrimaryFire()
{
    FireBullet();
}

void AE0LineTraceWeapon::Equip()
{
    Super::Equip();
    MeshComp->SetCastShadow(false);
    SetActorEnableCollision(false);
}

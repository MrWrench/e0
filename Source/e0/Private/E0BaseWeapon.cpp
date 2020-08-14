// Fill out your copyright notice in the Description page of Project Settings.


#include "E0BaseWeapon.h"

// Sets default values
AE0BaseWeapon::AE0BaseWeapon()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AE0BaseWeapon::BeginPlay()
{
    Super::BeginPlay();
}

// Called every frame
void AE0BaseWeapon::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}


void AE0BaseWeapon::StartPrimaryFire()
{
    FireInput = true;
}

void AE0BaseWeapon::StopPrimaryFire()
{
    FireInput = false;
}

void AE0BaseWeapon::Equip()
{
    OnEquip();
}

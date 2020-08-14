// Fill out your copyright notice in the Description page of Project Settings.


#include "E0BaseGadget.h"

// Sets default values
AE0BaseGadget::AE0BaseGadget()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AE0BaseGadget::BeginPlay()
{
	Super::BeginPlay();
	UseInput = false;
}

// Called every frame
void AE0BaseGadget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AE0BaseGadget::StartUsing()
{
	UseInput = true;
	OnStartUsing();
}

void AE0BaseGadget::StopUsing()
{
	UseInput = false;
	OnStopUsing();
}

void AE0BaseGadget::CancelUsing()
{
	UseInput = false;
	OnCancelUsing();
}

void AE0BaseGadget::Equip()
{
	OnEquip();
}


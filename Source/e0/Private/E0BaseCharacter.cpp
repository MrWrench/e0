// Fill out your copyright notice in the Description page of Project Settings.


#include "E0BaseCharacter.h"

// Sets default values
AE0BaseCharacter::AE0BaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AE0BaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AE0BaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AE0BaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


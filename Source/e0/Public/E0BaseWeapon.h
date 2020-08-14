// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "E0BaseWeapon.generated.h"

UCLASS(Abstract)
class E0_API AE0BaseWeapon : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    AE0BaseWeapon();
    UPROPERTY()
    bool IsFiring;

protected:
    UPROPERTY()
    bool FireInput;

    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    UFUNCTION(BlueprintCallable)
    virtual void StartPrimaryFire();

    UFUNCTION(BlueprintImplementableEvent)
    void OnStartPrimaryFire();
    
    UFUNCTION(BlueprintCallable)
    virtual void StopPrimaryFire();

    UFUNCTION(BlueprintImplementableEvent)
    void OnStopPrimaryFire();

    UFUNCTION(BlueprintCallable)
    virtual void Equip();
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void OnEquip();

    UFUNCTION(BlueprintPure)
    FORCEINLINE APawn* GetCharacterOwner() const {return Cast<APawn>(GetOwner());}
};

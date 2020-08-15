// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "E0BaseWeapon.h"
#include "E0LineTraceWeapon.generated.h"

class USkeletalMeshComponent;
class UDamageType;

/**
 * 
 */
UCLASS()
class E0_API AE0LineTraceWeapon : public AE0BaseWeapon
{
    GENERATED_BODY()

public:
    AE0LineTraceWeapon();

protected:
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components")
    USkeletalMeshComponent* MeshComp;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay")
    float ShootRange;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay")
    float Damage;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay")
    float ImpulseStrength;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay")
    TSubclassOf<UDamageType> DamageType;

    UFUNCTION(BlueprintCallable, Category = "Gameplay")
    void Fire();

    UFUNCTION(BlueprintImplementableEvent)
    void OnFire();
    
    UFUNCTION(BlueprintCallable, Category = "Gameplay")
    void FireBullet();

    UFUNCTION(BlueprintImplementableEvent)
    void OnFireBullet();

    virtual void StartPrimaryFire() override;

    virtual void Equip() override;
};

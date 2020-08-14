// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "E0BaseGadget.generated.h"

UCLASS(Abstract)
class E0_API AE0BaseGadget : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    AE0BaseGadget();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
    bool UseInput;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    UFUNCTION(BlueprintCallable)
    virtual void StartUsing();

    UFUNCTION(BlueprintCallable)
    virtual void StopUsing();

    UFUNCTION(BlueprintCallable)
    virtual void CancelUsing();

    UFUNCTION(BlueprintCallable)
    virtual void Equip();

    UFUNCTION(BlueprintNativeEvent)
    /**
     * @returns true if damage is blocked
     */
    bool OnCharacterGetPointDamage(float Damage, const class UDamageType* DamageType, FVector HitLocation, FVector HitNormal,
                            class UPrimitiveComponent* HitComponent, FName BoneName, FVector ShotFromDirection,
                            class AController* InstigatedBy, AActor* DamageCauser, const FHitResult& HitInfo);
    virtual bool OnCharacterGetPointDamage_Implementation(float Damage, const class UDamageType* DamageType, FVector HitLocation, FVector HitNormal,
                            class UPrimitiveComponent* HitComponent, FName BoneName, FVector ShotFromDirection,
                            class AController* InstigatedBy, AActor* DamageCauser, const FHitResult& HitInfo) { return false; }

protected:
    UFUNCTION(BlueprintImplementableEvent)
    void OnStartUsing();

    UFUNCTION(BlueprintImplementableEvent)
    void OnStopUsing();

    UFUNCTION(BlueprintImplementableEvent)
    void OnCancelUsing();

    UFUNCTION(BlueprintImplementableEvent)
    void OnEquip();
};

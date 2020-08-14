// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "E0BaseGadget.h"

#include "E0GravityGlove.generated.h"

class UPrimitiveComponent;
class UPhysicsHandleComponent;
class UArrowComponent;
class ACharacter;
class UDamageType;

/**
 * 
 */
UCLASS()
class E0_API AE0GravityGlove : public AE0BaseGadget
{
    GENERATED_BODY()


public:
    AE0GravityGlove();

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gravity Glove")
    float PushCooldown;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gravity Glove")
    float PullCooldown;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gravity Glove")
    float GrabDelay;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gravity Glove")
    float MaxRange;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gravity Glove")
    float MaxGrabRange;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Gravity Glove")
    bool IsGrabbing;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Gravity Glove")
    float NextPushTime;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Gravity Glove")
    float NextPullTime;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Gravity Glove")
    float GrabbedComponentMass;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gravity Glove")
    UCurveFloat* PullAcceleration;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gravity Glove")
    UCurveFloat* PushVelocity;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gravity Glove")
    float SelfPullAcceleration;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gravity Glove")
    UCurveFloat* SelfPushVelocity;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gravity Glove")
    float GrabJumpVelocity;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gravity Glove")
    float MaxPullMass;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gravity Glove")
    float MaxPushMass;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gravity Glove")
    float MaxError;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gravity Glove")
    float InitialGrabErrorTime;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gravity Glove")
    float GrabDistanceBBoxMultiplier;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gravity Glove")
    float MinGrabDistance;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gravity Glove")
    float GrabbedComponentMaxSpeed;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gravity Glove")
    float MinGrabAngle;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gravity Glove")
    float MaxGrabAngle;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Gravity Glove")
    UPhysicsHandleComponent* PhysicsHandleComponent;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Gravity Glove")
    UArrowComponent* ArrowComponent;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Gravity Glove")
    ACharacter* OwnerCharacter;

    UFUNCTION(BlueprintCallable, Category = "Gravity Glove")
    void PushComponent(UPrimitiveComponent* PushedComponent, FVector ForwardDirection, FName BoneName, float Distance);

    UFUNCTION(BlueprintCallable, Category = "Gravity Glove")
    void PushComponentAtLocation(UPrimitiveComponent* PushedComponent, FVector ForwardDirection, FVector Location, FName BoneName,
                                 float Distance);

    UFUNCTION(BlueprintPure, Category = "Gravity Glove")
    FVector GetPushImpulse(float Mass, const FVector Direction, const float Distance) const;

    UFUNCTION(BlueprintCallable, Category = "Gravity Glove")
    void GrabComponent(UPrimitiveComponent* GrabbedComponent, FName BoneName);

    UFUNCTION(BlueprintImplementableEvent, Category = "Gravity Glove")
    void OnGrabComponent(UPrimitiveComponent* GrabbedComponent, FName BoneName);

    UFUNCTION(BlueprintCallable, Category = "Gravity Glove")
    void ReleaseGrabbedComponent();

    UFUNCTION(BlueprintImplementableEvent, Category = "Gravity Glove")
    void OnReleaseGrabbedComponent(UPrimitiveComponent* GrabbedComponent);

    UFUNCTION(BlueprintCallable, Category = "Gravity Glove")
    void PullComponent(UPrimitiveComponent* PulledComponent, FVector ForwardDirection, FName BoneName, float Distance);

    UFUNCTION(BlueprintCallable, Category = "Gravity Glove")
    void DoGrabJump();

    UFUNCTION(BlueprintCallable, Category = "Gravity Glove")
    void DoSelfPush(const float Distance);

    UFUNCTION(BlueprintCallable, Category = "Gravity Glove")
    void DoSelfPull();

    float ComputeError();

    virtual void BeginPlay() override;

private:
    float CurrentGrabDistance;
    FRotator CurrentGrabRotation;
    float ErrorTime;
    float Error;
    float NextGrabTime;
    
public:
    virtual void StartUsing() override;
    virtual void StopUsing() override;
    FVector GetGrabTargetLocation() const;
    bool IsPulling() const;
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    UFUNCTION(BlueprintCallable)
    virtual bool OnCharacterGetPointDamage_Implementation(float Damage, const UDamageType* DamageType, FVector HitLocation,
                                                          FVector HitNormal,
                                                          UPrimitiveComponent* HitComponent, FName BoneName, FVector ShotFromDirection,
                                                          AController* InstigatedBy, AActor* DamageCauser,
                                                          const FHitResult& HitInfo) override;
};
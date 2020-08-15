// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "E0BaseCharacter.h"
#include "E0BaseGadget.h"
#include "e0Character.generated.h"

class UInputComponent;
class AE0BaseWeapon;
class AE0BaseGadget;

UCLASS(config=Game)
class Ae0Character : public AE0BaseCharacter
{
	GENERATED_BODY()

protected:
	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category=Mesh)
	class USkeletalMeshComponent* Mesh1P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCameraComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)	
	TArray<AE0BaseWeapon*> Weapons;
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = Gameplay, meta = (AllowPrivateAccess = "true"))
	AE0BaseWeapon* CurrentWeapon;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = Gameplay, meta = (AllowPrivateAccess = "true"))
	AE0BaseGadget* EquippedGadget;

	FVector ViewKick;

public:
	Ae0Character();

protected:
	virtual void BeginPlay();

public:
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Camera)
	float RecoilRecoverySpeed;

protected:
	
	UFUNCTION(BlueprintImplementableEvent, Category=Gameplay)
	void OnStartPrimaryFire();

	UFUNCTION(BlueprintImplementableEvent, Category=Gameplay)
	void OnStopPrimaryFire();

	UFUNCTION(BlueprintImplementableEvent, Category=Gameplay)
	void OnStartUsingGadget();

	UFUNCTION(BlueprintImplementableEvent, Category=Gameplay)
	void OnStopUsingGadget();

	UFUNCTION(BlueprintImplementableEvent, Category=Gameplay)
    void OnEquipWeapon(AE0BaseWeapon* NewWeapon);

	UFUNCTION(BlueprintImplementableEvent, Category=Gameplay)
    void OnSwitchWeapon(AE0BaseWeapon* NewWeapon);

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);
	
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	virtual void GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const override;

public:
	/** Returns Mesh1P subobject **/
	FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	UFUNCTION(BlueprintCallable)
	void SwitchWeapon(AE0BaseWeapon* NewWeapon);

	UFUNCTION(BlueprintCallable)
	void EquipWeapon(AE0BaseWeapon* NewWeapon);

	UFUNCTION(BlueprintCallable)
	void EquipGadget(AE0BaseGadget* NewGadget);

	UFUNCTION(BlueprintCallable)
	void StartPrimaryFire();
	UFUNCTION(BlueprintCallable)
	void StopPrimaryFire();
	
	UFUNCTION(BlueprintCallable)
	void StartUsingGadget();
	UFUNCTION(BlueprintCallable)
	void StopUsingGadget();

	UFUNCTION(BlueprintCallable, Category = Camera)
	void AddViewKick(FVector KickVector);

	virtual void Tick(float DeltaTime) override;
};


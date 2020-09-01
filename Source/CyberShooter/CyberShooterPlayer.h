// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CyberShooterPawn.h"
#include "CyberShooterPlayer.generated.h"

// A player controlled pawn with a camera and input setup
UCLASS(BlueprintType, Blueprintable)
class CYBERSHOOTER_API ACyberShooterPlayer : public ACyberShooterPawn
{
	GENERATED_BODY()
	
public:
	ACyberShooterPlayer();

	/// APawn Functions ///

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	/// IBreakable Functions ///

	virtual void Kill();

	/// Accessor functions ///

	FORCEINLINE class UCameraComponent* GetCamera() const { return CameraComponent; }
	FORCEINLINE class USpringArmComponent* GetSpringArm() const { return SpringArmComponent; }
	FORCEINLINE const TArray<UWeapon*>* GetWeaponSet() const { return &WeaponSet; }

	// Select a weapon from the available weapons
	UFUNCTION(BlueprintCallable)
		void SelectWeapon(int32 Slot);
	// Add a new weapon to the player's inventory
	UFUNCTION(BlueprintCallable)
		void AddWeapon(UWeapon* NewWeapon);
	// Stop firing the weapon when something is happening in the game
	UFUNCTION(BlueprintCallable)
		void StopFiring();
	// Set the player's gravity direction
	UFUNCTION(BlueprintCallable)
		bool SetOrientation(FVector NewForward, FVector NewUp);

protected:
	// Static names for axis bindings
	static const FName MoveForwardBinding;
	static const FName MoveRightBinding;

	// The forward vector for the player
	UPROPERTY(Category = Movement, EditAnywhere, BlueprintReadWrite)
		FVector Forward;
	// The up vector for the player
	UPROPERTY(Category = Movement, EditAnywhere, BlueprintReadWrite)
		FVector Up;

	// The weapons available to the player
	UPROPERTY(Category = Weapon, EditAnywhere, BlueprintReadWrite)
		TArray<UWeapon*> WeaponSet;

	// The direction the camera will point relative to the current orientation
	UPROPERTY(Category = Camera, EditAnywhere, BlueprintReadWrite)
		FRotator CameraAngle;
	// The game camera
	UPROPERTY(Category = Camera, VisibleAnywhere, BlueprintReadOnly)
		class UCameraComponent* CameraComponent;
	// The camera spring arm
	UPROPERTY(Category = Camera, VisibleAnywhere, BlueprintReadOnly)
		class USpringArmComponent* SpringArmComponent;

	// The movement component
	UPROPERTY(Category = Components, EditAnywhere, BlueprintReadOnly)
		class UPlayerMovementComponent* MovementComponent;
};

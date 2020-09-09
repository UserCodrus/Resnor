// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CyberShooterPawn.h"
#include "CyberShooterPlayer.generated.h"

class ATeleportBase;

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
	FORCEINLINE FVector GetForwardVector() const { return Forward; }
	FORCEINLINE FVector GetUpVector() const { return Up; }

	// Select a weapon from the available weapons
	UFUNCTION(BlueprintCallable)
		void SelectWeapon(int32 Slot);
	// Add a new weapon to the player's inventory
	UFUNCTION(BlueprintCallable)
		void AddWeapon(UWeapon* NewWeapon);

	// Set the player's gravity direction
	UFUNCTION(BlueprintCallable)
		bool SetOrientation(FVector NewForward, FVector NewUp, bool SnapCamera = false);
	// Verify the direction of the player's up vector
	UFUNCTION(BlueprintCallable)
		bool CheckOrientation(FVector RequiredUp);

	// Add a velocity impulse to the player
	UFUNCTION(BlueprintCallable)
		void AddImpulse(FVector Impulse);
	// Add a velocity impule relative to the player's orientation
	UFUNCTION(BlueprintCallable)
		void AddRelativeImpulse(FVector Impulse);

	// Teleport to a teleport target
	UFUNCTION(BlueprintCallable)
		void Teleport(ATeleportBase* Teleporter, ATeleportBase* Target);
	// Make sure the player can use a teleporter, returns false if the teleporter was used as a teleport target to prevent circular telports
	bool CheckTeleport(ATeleportBase* Target);

protected:
	// Transition the camera angle
	void CameraTransition(float DeltaTime);

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
	// The time it takes to shift camera angles when changing orientation
	UPROPERTY(Category = Camera, EditAnywhere, BlueprintReadWrite)
		float CameraTransitionTime;
	// The game camera
	UPROPERTY(Category = Camera, VisibleAnywhere, BlueprintReadOnly)
		class UCameraComponent* CameraComponent;
	// The camera spring arm
	UPROPERTY(Category = Camera, VisibleAnywhere, BlueprintReadOnly)
		class USpringArmComponent* SpringArmComponent;

	// The movement component
	UPROPERTY(Category = Components, EditAnywhere, BlueprintReadOnly)
		class UPlayerMovementComponent* MovementComponent;

	// The last target the player telported to
	ATeleportBase* LastTeleportTarget;
	// The starting rotation for a camera transition
	FQuat CameraStartRotation;
	// The desired rotation for a camera transition
	FQuat CameraDesiredRotation;
	// The transition timer
	float CameraTimer;
};

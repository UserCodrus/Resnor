// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "PhysicsInterface.h"

#include "CoreMinimal.h"
#include "CyberShooterPawn.h"
#include "CyberShooterPlayer.generated.h"

class ATeleportBase;

// A player controlled pawn with a camera and input setup
UCLASS(BlueprintType, Blueprintable)
class CYBERSHOOTER_API ACyberShooterPlayer : public ACyberShooterPawn, public IPhysicsInterface
{
	GENERATED_BODY()
	
public:
	ACyberShooterPlayer();

	/// APawn Functions ///

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	virtual FVector GetVelocity() const override;

	// Apply physics impulses on hitting obstacles
	UFUNCTION()
		virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/// ICombatInterface ///

	void Kill() override;

	/// IPhysicsInterface ///

	void AddImpulse(FVector Force) override;
	void AddRelativeImpulse(FVector Force) override;
	void AddStaticForce(FVector Force) override;
	void RemoveStaticForce(FVector Force) override;
	void ResetStaticForce() override;
	void SetGravityEnabled(bool Enable) override;
	void SetGravity(float NewGravity) override;
	void ResetGravity() override;
	void SetAirFriction(float NewFriction) override;
	void ResetAirFriction() override;
	void SetFriction(float NewFriction) override;
	void ResetFriction() override;
	void SetTickRate(float NewRate) override;

	/// Blueprint interface ///
	
	UFUNCTION(BlueprintCallable)
		void BPAddImpulse(FVector Force);
	UFUNCTION(BlueprintCallable)
		void BPAddRelativeImpulse(FVector Force);

	/// Accessor functions ///

	FORCEINLINE class UCameraComponent* GetCamera() const { return CameraComponent; }
	FORCEINLINE class USpringArmComponent* GetSpringArm() const { return SpringArmComponent; }
	FORCEINLINE const TArray<UWeapon*>* GetWeaponSet() const { return &WeaponSet; }
	FORCEINLINE const TArray<UAbility*>* GetAbilitySet() const { return &AbilitySet; }

	// Determine if the player is in the air
	UFUNCTION(BlueprintPure)
		bool IsFalling() const;
	// Returns true if the player is not on a moving platform
	UFUNCTION(BlueprintPure)
		bool IsStable() const;

	// Select a weapon from the available weapons
	UFUNCTION(BlueprintCallable)
		void SelectWeapon(int32 Slot);
	// Add a new weapon to the player's inventory
	UFUNCTION(BlueprintCallable)
		void AddWeapon(UWeapon* NewWeapon);
	// Select an ability from the player's available abilities
	UFUNCTION(BlueprintCallable)
		void SelectAbility(int32 Slot);
	// Add a new ability to the player
	UFUNCTION(BlueprintCallable)
		void AddAbility(UAbility* NewAbility);

	// Set the player's gravity direction
	UFUNCTION(BlueprintCallable)
		bool SetOrientation(FVector NewForward, FVector NewUp, bool SnapCamera = false);
	// Revert to the player's previous orientation
	UFUNCTION(BlueprintCallable)
		bool RevertOrientation(bool SnapCamera = false);
	// Verify the direction of the player's up vector
	UFUNCTION(BlueprintCallable)
		bool CheckOrientation(FVector RequiredUp) const;

	// Increase or decrease player speed
	UFUNCTION(BlueprintCallable)
		void SetSpeedMultiplier(float NewValue);
	// Increase or decrease player gravity
	UFUNCTION(BlueprintCallable)
		void SetGravityMultiplier(float NewValue);
	// Stops the effects of gravity
	UFUNCTION(BlueprintCallable)
		void StopGravity();
	// Halt player movement
	UFUNCTION(BlueprintCallable)
		void StopMovement();

	// Teleport to a teleport target
	UFUNCTION(BlueprintCallable)
		void Teleport(ATeleportBase* Teleporter, ATeleportBase* Target);
	// Make sure the player can use a teleporter, returns false if the teleporter was used as a teleport target to prevent circular telports
	bool CheckTeleport(ATeleportBase* Target);

protected:
	// Transition the camera angle
	void CameraTransition(float DeltaTime);

	/// Properties ///
	
	// Static names for axis bindings
	static const FName MoveForwardBinding;
	static const FName MoveRightBinding;

	// The forward vector before the last orientation change
	UPROPERTY(Category = "Movement|Orientation", EditAnywhere, BlueprintReadWrite)
		FVector PreviousForward;
	// The up vector before the last orientation change
	UPROPERTY(Category = "Movement|Orientation", EditAnywhere, BlueprintReadWrite)
		FVector PreviousUp;
	// The speed multiplier for player movement
	UPROPERTY(Category = "Movement|Physics", EditAnywhere, BlueprintReadWrite)
		float SpeedMultiplier;
	// The gravity modification from abilities
	UPROPERTY(Category = "Movement|Physics", EditAnywhere, BlueprintReadWrite)
		float GravityMultiplier;
	// The acceleration of gravity in the world
	UPROPERTY(Category = "Movement|Physics", VisibleAnywhere, BlueprintReadWrite)
		float WorldGravity;

	// The weapons available to the player
	UPROPERTY(Category = "Weapon", EditAnywhere, BlueprintReadWrite)
		TArray<UWeapon*> WeaponSet;
	// The abilities the player has learned
	UPROPERTY(Category = "Ability", EditAnywhere, BlueprintReadWrite)
		TArray<UAbility*> AbilitySet;

	// The direction the camera will point relative to the current orientation
	UPROPERTY(Category = "Camera", EditAnywhere, BlueprintReadWrite)
		FRotator CameraAngle;
	// The time it takes to shift camera angles when changing orientation
	UPROPERTY(Category = "Camera", EditAnywhere, BlueprintReadWrite)
		float CameraTransitionTime;
	// The game camera
	UPROPERTY(Category = "Components", VisibleAnywhere, BlueprintReadOnly)
		class UCameraComponent* CameraComponent;
	// The camera spring arm
	UPROPERTY(Category = "Components", VisibleAnywhere, BlueprintReadOnly)
		class USpringArmComponent* SpringArmComponent;

	// The movement component
	UPROPERTY(Category = "Components", EditAnywhere, BlueprintReadOnly)
		class UPlayerMovementComponent* MovementComponent;

	// The last target the player telported to
	ATeleportBase* LastTeleportTarget;
	// The starting rotation for a camera transition
	FQuat CameraStartRotation;
	// The desired rotation for a camera transition
	FQuat CameraDesiredRotation;
	// The starting position for a camera transition
	FVector CameraStartPosition;
	// The desired position for a camera transition
	FVector CameraDesiredPosition;
	// Set to true when the position needs to translate during a camera transition
	bool TransitionPosition;
	// The transition timer
	float CameraTimer;
	// The timer for taking damage from floors
	float FloorDamageTimer;
};

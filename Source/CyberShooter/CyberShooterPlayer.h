// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "PhysicsInterface.h"
#include "OrientationInterface.h"

#include "CoreMinimal.h"
#include "CyberShooterPawn.h"
#include "CyberShooterPlayer.generated.h"

class ATeleportBase;

// A set of respawn data
struct FRespawnPoint
{
	FVector Location;
	FVector Forward;
	FVector Up;
};

// A player controlled pawn with a camera and input setup
UCLASS(BlueprintType, Blueprintable)
class CYBERSHOOTER_API ACyberShooterPlayer : public ACyberShooterPawn, public IPhysicsInterface, public IOrientationInterface
{
	GENERATED_BODY()
	
public:
	ACyberShooterPlayer();

	/// APawn Functions ///

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	virtual FVector GetVelocity() const override;

	// Apply physics impulses when hitting obstacles
	UFUNCTION()
		virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/// ICombatInterface ///

	bool Damage(int32 Value, int32 DamageType, UForceFeedbackEffect* RumbleEffect, UPrimitiveComponent* HitComp = nullptr, AActor* Source = nullptr, AActor* Origin = nullptr) override;
	void Kill() override;

	/// IPhysicsInterface ///

	FVector GetVelocity() override;
	void ChangeVelocity(FVector NewVelocity) override;
	bool CanMove() override;
	void AddImpulse(FVector Force) override;
	void AddRelativeImpulse(FVector Force) override;
	void AddStaticForce(FVector Force) override;
	void RemoveStaticForce(FVector Force) override;
	void ResetStaticForce() override;
	void SetGravity(float NewGravity) override;
	void ResetGravity() override;
	void SetAirFriction(float NewFriction) override;
	void ResetAirFriction() override;
	void SetFriction(float NewFriction) override;
	void ResetFriction() override;
	void SetMass(float NewMass) override;
	void ResetMass() override;
	void SetTickRate(float NewRate) override;
	float GetMass() const override;
	float GetWeight() const override;

	/// IOrientationInterface ///

	bool SetOrientation(FVector NewForward, FVector NewUp) override;
	bool CheckOrientation(FVector DesiredUp) override;

	/// Blueprint interface ///
	
	// Add a velocity impulse relative to world space
	UFUNCTION(BlueprintCallable)
		void BPAddImpulse(FVector Force);
	// Add a velocity impulse relative to local space
	UFUNCTION(BlueprintCallable)
		void BPAddRelativeImpulse(FVector Force);

	/// Accessor functions ///

	FORCEINLINE class UCameraComponent* GetCamera() const { return CameraComponent; }
	FORCEINLINE class USpringArmComponent* GetSpringArm() const { return SpringArmComponent; }
	FORCEINLINE const TArray<UWeapon*>& GetWeaponSet() const { return WeaponSet; }
	FORCEINLINE const TArray<UAbility*>& GetAbilitySet() const { return AbilitySet; }
	FORCEINLINE const int32 GetKeys() const { return Keys; }
	FORCEINLINE const bool HasMoved() const { return Moved; }

	// Load player data from a save object
	bool LoadData(const class UCyberShooterSave* Save);

	// Increase max health
	UFUNCTION(BlueprintCallable)
		void UpgradeHealth();
	// Increase max momentum
	UFUNCTION(BlueprintCallable)
		void UpgradeMomentum();
	// Add a data key
	UFUNCTION(BlueprintCallable)
		void AddKey();

	// Refill the player's stats
	void Refill();
	
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
	// Get the selected weapon slot
	UFUNCTION(BlueprintPure)
		int32 GetSelectedWeapon();
	// Select an ability from the player's available abilities
	UFUNCTION(BlueprintCallable)
		void SelectAbility(int32 Slot);
	// Add a new ability to the player
	UFUNCTION(BlueprintCallable)
		void AddAbility(UAbility* NewAbility);
	// Get the selected ability slot
	UFUNCTION(BlueprintPure)
		int32 GetSelectedAbility();

	// Disable abilities
	void DisableAbility();
	// Enable abilities
	void EnableAbility();

	// Enable ghost collision
	UFUNCTION(BlueprintCallable)
		void EnableGhostCollision();
	// Disable ghost collision
	UFUNCTION(BlueprintCallable)
		void DisableGhostCollision();
	// Start using the warp ability
	UFUNCTION(BlueprintCallable)
		void StartWarp();
	// Stop using the warp ability
	UFUNCTION(BlueprintCallable)
		void EndWarp();

	// Increase or decrease player speed
	UFUNCTION(BlueprintCallable)
		void SetSpeedMultiplier(float NewValue);
	// Increase or decrease player gravity
	UFUNCTION(BlueprintCallable)
		void SetGravityMultiplier(float NewValue);
	// Increase or decrease player mass
	UFUNCTION(BlueprintCallable)
		void SetMassMultiplier(float NewValue);
	// Increase or decrease player friction
	UFUNCTION(BlueprintCallable)
		void SetFrictionMultiplier(float NewValue);
	// Stops the effects of gravity
	UFUNCTION(BlueprintCallable)
		void StopGravity();
	// Halt player movement
	UFUNCTION(BlueprintCallable)
		void StopMovement();

	// Notify the player when a ghost zone is entered
	void EnterGhostZone();
	// Notify the player when a ghost zone is exited
	void ExitGhostZone();

	// Change the camera distance
	void SetCameraDistance(float Distance);
	// Change the camera distance back to default
	void ResetCameraDistance();

	// Teleport to a teleport target
	UFUNCTION(BlueprintCallable)
		void Teleport(ATeleportBase* Teleporter, ATeleportBase* Target);
	// Make sure the player can use a teleporter, returns false if the teleporter was used as a teleport target to prevent circular telports
	bool CheckTeleport(ATeleportBase* Target);

	// Get the location of the player's current respawn point
	UFUNCTION(BlueprintPure)
		FVector GetRespawnLocation();
	// Force the player to respawn
	UFUNCTION(BlueprintCallable)
		void ForceRespawn();

protected:
	// Apply a specific orientation to the player
	bool ApplyOrientation(FVector NewForward, FVector NewUp, bool SnapCamera = false);
	// Transition the camera angle
	void CameraTransition(float DeltaTime, FVector MovementDirection);
	// Set the respawn point of the player
	inline void SetRespawn();
	// Get the player's current respawn point
	inline FRespawnPoint& GetRespawn();
	// Force the player to respawn
	void Respawn();

	/// Properties ///
	
	// Static names for axis bindings
	static const FName MoveForwardBinding;
	static const FName MoveRightBinding;

	// Save data for debugging
	UPROPERTY(EditAnywhere)
		class UCyberShooterGameInstance* GameInstanceData;

	// The invincibility duration after respawning
	UPROPERTY(Category = "Attributes|Damage", EditAnywhere)
		float RespawnCooldown;
	// The number of keys the player has collected
	UPROPERTY(Category = "Attributes", EditAnywhere)
		int32 Keys;

	// The speed multiplier for player movement
	UPROPERTY(Category = "Movement|Physics", EditAnywhere)
		float SpeedMultiplier;
	// The gravity modification from abilities
	UPROPERTY(Category = "Movement|Physics", EditAnywhere)
		float GravityMultiplier;
	// The acceleration of gravity in the world
	UPROPERTY(Category = "Movement|Physics", VisibleAnywhere)
		float WorldGravity;
	// The mass of the pawn
	UPROPERTY(Category = "Movement|Physics", EditAnywhere)
		float Mass;
	// The mass modification from abilities
	UPROPERTY(Category = "Movement|Physics", EditAnywhere)
		float MassMultiplier;
	// The base world friction
	UPROPERTY(Category = "Movement|Physics", EditAnywhere)
		float Friction;
	// The friction modification from abilities
	UPROPERTY(Category = "Movement|Physics", EditAnywhere)
		float FrictionMultiplier;

	// The maximum distance the player can move before respawning
	UPROPERTY(Category = "Movement|Respawn", EditAnywhere)
		float RespawnDistance;

	// The weapons available to the player
	UPROPERTY(Category = "Weapon", EditAnywhere, BlueprintReadWrite)
		TArray<UWeapon*> WeaponSet;
	// The abilities the player has learned
	UPROPERTY(Category = "Ability", EditAnywhere, BlueprintReadWrite)
		TArray<UAbility*> AbilitySet;

	// The collision profile for the ghost ability
	UPROPERTY(Category = "Ability", EditAnywhere)
		TEnumAsByte<ECollisionChannel> GhostCollision;

	// The direction the camera will point relative to the current orientation
	UPROPERTY(Category = "Camera", EditAnywhere)
		FRotator CameraAngle;
	// The default distance the camera will be from the player outside an aggro zone
	UPROPERTY(Category = "Camera", EditAnywhere)
		float CameraDistance;
	// The distance the camera should be from the player currently
	UPROPERTY(Category = "Camera", EditInstanceOnly)
		float DesiredDistance;
	// The speed the camera will zoom in or out per second
	UPROPERTY(Category = "Camera", EditAnywhere)
		float CameraZoomSpeed;
	// The camera lag speed
	UPROPERTY(Category = "Camera", EditAnywhere)
		float CameraLag;
	// The time it takes to shift camera angles when changing orientation
	UPROPERTY(Category = "Camera", EditAnywhere)
		float CameraTransitionTime;
	// The material parameters used for camera occlusion
	UPROPERTY(Category = "Camera", EditAnywhere)
		class UMaterialParameterCollection* CameraParameterCollection;
	// The parameter instance we will be editing
	UPROPERTY(Category = "Camera", VisibleAnywhere)
		class UMaterialParameterCollectionInstance* CameraParameters;
	// The maximum radius of the occlusion capsule
	UPROPERTY(Category = "Camera", VisibleAnywhere)
		float OcclusionRadius;
	// The amount of time it takes to dilate the occlusion radius
	UPROPERTY(Category = "Camera", VisibleAnywhere)
		float OcclusionTime;

	// The game camera
	UPROPERTY(Category = "Components", VisibleAnywhere)
		class UCameraComponent* CameraComponent;
	// The camera spring arm
	UPROPERTY(Category = "Components", VisibleAnywhere)
		class USpringArmComponent* SpringArmComponent;
	// The collision capsule for the warp dummy
	UPROPERTY(Category = "Components", VisibleDefaultsOnly, BlueprintReadOnly)
		class UCapsuleComponent* DummyCollisionComponent;

	// The movement component
	UPROPERTY(Category = "Components", EditAnywhere)
		class UPlayerMovementComponent* MovementComponent;

	// Particles that spawn when entering a teleport
	UPROPERTY(Category = "Effects|Particles", EditAnywhere)
		UParticleSystem* TeleportStartParticles;
	// Particles that spawn when leaving a teleport
	UPROPERTY(Category = "Effects|Particles", EditAnywhere)
		UParticleSystem* TeleportEndParticles;
	// Force feedback played when hitting a physics object
	UPROPERTY(Category = "Effects|Feedback", EditAnywhere)
		UForceFeedbackEffect* ImpactRumble;
	// The rumble effect that applies when falling out of bounds
	UPROPERTY(Category = "Effects|Feedback", EditAnywhere)
		UForceFeedbackEffect* FallRumble;
	// The rumble effect that applies when taking damage from a floor
	UPROPERTY(Category = "Effects|Feedback", EditAnywhere)
		UForceFeedbackEffect* EnvironmentRumble;

	// A set of respawn points
	TArray<FRespawnPoint> RespawnPoints;
	// The current respawn point we are using
	int32 CurrentRespawn;

	// The collision channel used by default
	TEnumAsByte<ECollisionChannel> DefaultCollision;
	// The last target the player telported to
	ATeleportBase* LastTeleportTarget;
	// The starting rotation for a camera transition
	FQuat CameraStartRotation;
	// The desired rotation for a camera transition
	FQuat CameraDesiredRotation;
	// The initial rotation of the player before a camera transition
	FQuat PlayerStartRotation;
	// The starting position for a camera transition
	FVector CameraStartPosition;
	// Set to true when the position needs to translate during a camera transition
	bool TransitionPosition;
	// The transition timer
	float CameraTimer;
	// The timer for taking damage from floors
	float FloorDamageTimer;
	// The number of layers of ghost zones the player has entered
	int32 GhostZones;
	// Set to true when the player has warp active
	bool Warp;
	// Set to true when the player moves for the first time after loading a level, used to ignore triggers that may activate when loading
	bool Moved;
	// The size of the occlusion mask currently
	float OcclusionSize;
};

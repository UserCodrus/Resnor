// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "PhysicsInterface.h"
#include "OrientationInterface.h"
#include "AggroInterface.h"

#include "CoreMinimal.h"
#include "PhysicalStaticMesh.h"
#include "PhysicsObject.generated.h"

// A static mesh object with physics enabled
UCLASS()
class CYBERSHOOTER_API APhysicsObject : public APhysicalStaticMesh, public IPhysicsInterface, public IOrientationInterface, public IAggroInterface
{
	GENERATED_BODY()
	
public:
	APhysicsObject();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual FVector GetVelocity() const override;

	// Apply physics impulses on hitting obstacles
	UFUNCTION()
		virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

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
	void SetMass(float Multiplier) override;
	void ResetMass() override;
	void SetTickRate(float NewRate) override;
	float GetMass() const override;
	float GetWeight() const override;

	/// IOrientationInterface ///

	bool SetOrientation(FVector NewForward, FVector NewUp) override;
	bool CheckOrientation(FVector DesiredUp) override;

	/// IAggroInterface ///

	virtual void Aggro() override;
	virtual void EndAggro() override;
	virtual bool IsAggro() override;
	virtual void AggroReset() override;
	virtual void EnterAggro() override;
	virtual void ExitAggro() override;
	virtual void AggroDisable() override {};
	virtual void RegisterZone(class AAggroZone* Zone) override {};

	/// Blueprint Events ///

	// Called when the object is enabled
	UFUNCTION(BlueprintImplementableEvent)
		void EnableObject();
	// Called when the object is disabled
	UFUNCTION(BlueprintImplementableEvent)
		void DisableObject();

	/// Physics Object Functions ///

	// Enable the object
	UFUNCTION(BlueprintCallable)
		void Enable();
	// Freeze the object
	UFUNCTION(BlueprintCallable)
		void Disable();

protected:
	// Start the respawn timer for the object
	void StartRespawn();
	// Cancel the respawn timer
	void CancelRespawn();
	// Respawn the object
	void Respawn();
	// Internal implementation for orientation change
	bool SetOrientation_Internal(FVector NewForward, FVector NewUp);

	/// Properties ///

	// The mass of the pawn
	UPROPERTY(Category = "PhysicsProperties|Weight", EditAnywhere)
		float Mass;

	// If set to true, the object will be able to change orientation
	UPROPERTY(Category = "Orientation", EditAnywhere)
		bool CanChangeOrientation;
	// The forward vector the object uses when respawning
	UPROPERTY(Category = "Orientation", EditAnywhere)
		FVector InitialForward;
	// The up vector the object uses when respawning
	UPROPERTY(Category = "Orientation", EditAnywhere)
		FVector InitialUp;

	// The point the object will return to when respawning
	UPROPERTY(Category = "Respawn", VisibleAnywhere)
		FVector RespawnPoint;
	// Set to true if the object has just respawned
	UPROPERTY(Category = "Respawn", EditAnywhere)
		bool Respawned;
	// The time it takes for the object to respawn after leaving its zone
	UPROPERTY(Category = "Respawn", EditAnywhere)
		float RespawnDuration;
	// The time it takes the object to spawn in after respawning
	UPROPERTY(Category = "Respawn", EditAnywhere)
		float RespawnCooldown;
	// The blink rate when the object is respawning
	UPROPERTY(Category = "Respawn", EditAnywhere)
		int32 BlinkRate;

	// If set to true, the object will start in a disabled state
	UPROPERTY(Category = "Respawn", EditAnywhere)
		bool StartLocked;

	// The movement component that handles physics
	UPROPERTY(Category = "Components", VisibleAnywhere)
		class UPhysicsMovementComponent* MovementComponent;

	// The timer used to track respawns
	float RespawnTimer;
};

// A physics object that adjusts its gravity direction to match the player
UCLASS()
class CYBERSHOOTER_API AMimicObject : public APhysicsObject
{
	GENERATED_BODY()

public:
	AMimicObject();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	/// IOrientationInterface ///

	bool SetOrientation(FVector NewForward, FVector NewUp) override;

	/// IAggroInterface ///

	void Aggro() override;
	void EndAggro() override;
	bool IsAggro() override;

protected:
	// A reference to the player used to check for orientation changes
	UPROPERTY(VisibleAnywhere)
		class ACyberShooterPlayer* Player;
	// An aggro counter used to prevent the object from copying the player when the player is too far away
	UPROPERTY(VisibleAnywhere)
		int32 AggroLevel;
};
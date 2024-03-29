// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "PhysicsInterface.h"
#include "AggroInterface.h"
#include "EnemyAIController.h"

#include "CoreMinimal.h"
#include "CyberShooterPawn.h"
#include "CyberShooterEnemy.generated.h"

class ASpawner;

// The base class for enemies
UCLASS(Abstract, BlueprintType, Blueprintable)
class CYBERSHOOTER_API AEnemyBase : public ACyberShooterPawn, public IAggroInterface
{
	GENERATED_BODY()
	
public:
	AEnemyBase();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	// The function that handles the enemy hitting obstacles
	UFUNCTION()
		virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	// This blueprint script is responsible for handling changes to AI settings
	UFUNCTION(BlueprintImplementableEvent)
		void AIChange(AEnemyAIController* PawnController, bool HealthChanged, bool MomentumChanged, bool DistanceChanged, bool AggroChanged);

	// Trigger the respawn timer for this enemy
	void StartRespawn();
	// Cancel the respawn timer
	virtual void CancelRespawn();
	// Respawn the enemy
	virtual void Respawn();
	// Apply contact damage to a player
	void ApplyImpact(class ACyberShooterPlayer* Player, UPrimitiveComponent* Comp);

	/// ICombatInterface ///

	virtual bool Damage(int32 Value, int32 DamageType, UForceFeedbackEffect* RumbleEffect, UPrimitiveComponent* HitComp = nullptr, AActor* Source = nullptr, AActor* Origin = nullptr) override;
	virtual void Kill() override;

	/// IAggroInterface ///

	virtual void Aggro() override;
	virtual void EndAggro() override;
	virtual bool IsAggro() override;
	virtual void EnterAggro() override;
	virtual void ExitAggro() override;
	virtual void AggroReset() override;
	virtual void AggroDisable() override;
	virtual void RegisterZone(class AAggroZone* Zone) override;

	/// Accessors ///
	
	FORCEINLINE int32 GetAggro() { return AggroLevel; }
	FORCEINLINE float GetAIDistance() { return AIDistance; }

	// Get the direction the pawn is aiming
	FVector GetAimVector() const;
	// Get the rotation of the pawn's aim component in world space
	FRotator GetAimRotation() const;
	// Get the rotation of the pawn's core component in local space
	FRotator GetAimLocalRotation() const;

	// Set the rotation of the aim component in wolrd space
	void SetAimRotation(FRotator Rotation);
	void SetAimRotation(FQuat Rotation);
	// Set the rotation of the aim component in wolrd space
	void SetAimWorldRotation(FRotator Rotation);
	void SetAimWorldRotation(FQuat Rotation);

	// Disable the pawn
	virtual void DisablePawn() override;
	// Enable the pawn
	virtual void EnablePawn() override;

	// Order the pawn to move in a direction relative to its orientation
	virtual void AddControlInput(FVector Direction) {};

protected:
	/// Utilities ///

	// Check to see if the pawn should activate its AI
	void CheckAIEnable();
	// Check to see if the pawn should disable its AI
	void CheckAIDisable();

	/// Blueprint Events ///
	
	// Called when the pawn's AI activates
	UFUNCTION(BlueprintImplementableEvent)
		void OnAggro();
	// Called when the pawn's AI deactivates
	UFUNCTION(BlueprintImplementableEvent)
		void OnEndAggro();

	/// AI ///

	// The aggro counter for the enemy
	UPROPERTY(Category = "AI|Aggro", EditAnywhere)
		int32 AggroLevel;
	// The aggro amount required to activate the enemy AI
	UPROPERTY(Category = "AI|Aggro", EditAnywhere)
		int32 RequiredAggro;
	// The aggro required to deactive AI
	UPROPERTY(Category = "AI|Aggro", EditAnywhere)
		int32 MinimumAggro;

	// The minimum distance change required to change the pawn's AI profile
	UPROPERTY(Category = "AI|Aggro", EditAnywhere)
		float AIDistance;
	// The zones that this enemy belongs to
	UPROPERTY(Category = "AI|Aggro", EditAnywhere)
		TArray<AAggroZone*> ParentZone;

	// If set to true, enemies will check if they are inside an obstacle before respawning
	UPROPERTY(Category = "AI|Respawning", EditAnywhere)
		bool SafeRespawn;
	// If set to true, the enemy will respawn when outside its aggro zone, otherwise it will be killed
	UPROPERTY(Category = "AI|Respawning", EditAnywhere)
		bool OutOfBoundsRespawn;
	// Set to true if the enemy has just respawned
	UPROPERTY(Category = "AI|Respawning", EditAnywhere)
		bool Respawned;
	// The time it takes for the enemy to respawn
	UPROPERTY(Category = "AI|Respawning", EditAnywhere)
		float RespawnDuration;
	// The time it takes the enemy to become active after respawning
	UPROPERTY(Category = "AI|Respawning", EditAnywhere)
		float RespawnCooldown;

	// The default aiming settings for the pawn
	UPROPERTY(Category = "AI|Settings", EditAnywhere)
		FAimProfile DefaultAiming;
	// The default movement settings for the pawn
	UPROPERTY(Category = "AI|Settings", EditAnywhere)
		FMovementProfile DefaultMovement;

	/// Movement ///
	
	// The force mutiplier for physics collisions
	UPROPERTY(Category = "Movement|Physics", EditAnywhere)
		float CollisionForce;

	/// Combat ///

	// Damage the enemy deals on contact to players
	UPROPERTY(Category = "Attributes|Damage", EditAnywhere, BlueprintReadWrite)
		int32 ContactDamage;

	// The weapon that will be fired when the pawn dies
	UPROPERTY(Category = "Weapon", EditAnywhere, BlueprintReadWrite)
		UWeapon* DeathWeapon;

	/// Components ///
	
	// The component that controls the pawn's weapon aiming
	UPROPERTY(Category = "Components", VisibleDefaultsOnly, BlueprintReadOnly)
		USceneComponent* AimComponent;

	// The timer for respawns
	float RespawnTimer;
};

// An enemy that can only turn and shoot
UCLASS(BlueprintType, Blueprintable)
class CYBERSHOOTER_API AEnemyTurret : public AEnemyBase
{
	GENERATED_BODY()

public:
	AEnemyTurret();

	virtual void BeginPlay() override;
};

// A turret that travels along a fixed path
UCLASS(BlueprintType, Blueprintable)
class CYBERSHOOTER_API AEnemyMovingTurret : public AEnemyTurret
{
	GENERATED_BODY()

public:
	AEnemyMovingTurret();

	virtual FVector GetVelocity() const override;

	/// Spline Utilities ///

	// Move the enemy to the start point of the spline
	UFUNCTION(Category = "SplineUtilities", CallInEditor)
		void MoveEnemyToStart();
	// Move the enemy to the end point of the spline
	UFUNCTION(Category = "SplineUtilities", CallInEditor)
		void MoveEnemyToEnd();
	// Erase all the spline's points and place it at the center of the enemy
	UFUNCTION(Category = "SplineUtilities", CallInEditor)
		void ResetSpline();
	// Move the spline's start to the center of the enemy
	UFUNCTION(Category = "SplineUtilities", CallInEditor)
		void MoveSpline();
	// Add a new spline point at the enemies current position
	UFUNCTION(Category = "SplineUtilities", CallInEditor)
		void AppendSpline();

protected:
	/// Properties ///
	
	// The spline component that dictates the turret's patrol path
	UPROPERTY(Category = "Components", EditAnywhere)
		class USplineComponent* Spline;
	// The movement component for the turret
	UPROPERTY(Category = "Components", EditAnywhere)
		class USplineMovementComponent* MovementComponent;
};

// An enemy that can both move and shoot
UCLASS(BlueprintType, Blueprintable)
class CYBERSHOOTER_API AEnemySeeker : public AEnemyBase, public IPhysicsInterface
{
	GENERATED_BODY()

public:
	AEnemySeeker();

	virtual void BeginPlay() override;
	virtual FVector GetVelocity() const override;

	virtual void CancelRespawn() override;
	virtual void Respawn() override;

	// Handle the seeker hitting physics objects
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	// Order the pawn to move in a direction
	virtual void AddControlInput(FVector Direction);

	virtual void DisablePawn() override;
	virtual void EnablePawn() override;

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

	/// IAggroInterface ///

	virtual void Aggro() override;
	virtual void EndAggro() override;

	/// Blueprint interface ///

	// Add a velocity impulse relative to world space
	UFUNCTION(BlueprintCallable)
		void BPAddImpulse(FVector Force);
	// Add a velocity impulse relative to local space
	UFUNCTION(BlueprintCallable)
		void BPAddRelativeImpulse(FVector Force);
	// Returns true if the pawn is in the air
	UFUNCTION(BlueprintCallable)
		bool BPIsFalling();

protected:
	/// Properties ///

	// The gravity modification from abilities
	UPROPERTY(Category = "Movement|Physics", EditAnywhere)
		float GravityMultiplier;
	// The acceleration of gravity in the world
	UPROPERTY(Category = "Movement|Physics", VisibleAnywhere)
		float WorldGravity;
	// The mass of the pawn
	UPROPERTY(Category = "Movement|Physics", EditAnywhere)
		float Mass;

	// The location the enemy will return to when respawning
	UPROPERTY()
		FVector RespawnLocation;
	
	// The movement component for the seeker
	UPROPERTY(Category = "Components", EditAnywhere)
		class UEnemyMovementComponent* MovementComponent;
};

// A seeker that is spawned from an enemy spawner
UCLASS(BlueprintType, Blueprintable)
class CYBERSHOOTER_API AEnemyMinion : public AEnemySeeker
{
	GENERATED_BODY()

public:
	AEnemyMinion();

	virtual void BeginPlay() override;

	// Claim ownership of this minion
	void ClaimPawn(ASpawner* NewParent);
	// Check to see if an actor is the parent of this minion
	bool IsParent(AActor* Actor);
	// Set the aggro level of the pawn
	void SetAggro(int32 Required, int32 Minimum);

	// Kill the pawn without making a sound when respawning
	void QuietKill();

	/// ICombatInterface ///

	virtual void Kill() override;

protected:
	// The spawner that spawned this minion
	UPROPERTY(VisibleAnywhere)
		ASpawner* Parent;
};

// An enemy that bounces off of walls and obstacles
UCLASS(BlueprintType, Blueprintable)
class CYBERSHOOTER_API AEnemyBouncer : public AEnemyBase
{
	GENERATED_BODY()

public:
	AEnemyBouncer();

	virtual void BeginPlay() override;

protected:
	// The movement component for the spike
	UPROPERTY(Category = "Components", EditAnywhere)
		class UBulletMovementComponent* MovementComponent;
};
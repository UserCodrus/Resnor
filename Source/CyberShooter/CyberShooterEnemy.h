// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CyberShooterPawn.h"
#include "CyberShooterEnemy.generated.h"

// A standard enemy type
UCLASS(Abstract, BlueprintType, Blueprintable)
class CYBERSHOOTER_API AEnemyBase : public ACyberShooterPawn
{
	GENERATED_BODY()
	
public:
	AEnemyBase();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	// The function that handles the enemy hitting obstacles
	UFUNCTION()
		virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/// ICombatInterface ///

	virtual void Kill() override;

	/// Accessors ///
	
	FORCEINLINE float GetAggroDistance() { return AggroDistance; }

protected:
	/// AI ///

	// The distance from which the enemy will begin to attack the player
	UPROPERTY(Category = "AI", EditAnywhere)
		float AggroDistance;

	/// Combat ///

	// Damage the enemy deals on contact to players
	UPROPERTY(Category = "Attributes|Damage", EditAnywhere, BlueprintReadWrite)
		int32 ContactDamage;

	// The weapon that will be fired when the pawn dies
	UPROPERTY(Category = "Weapon", EditAnywhere, BlueprintReadWrite)
		UWeapon* DeathWeapon;
};

UCLASS(BlueprintType, Blueprintable)
class CYBERSHOOTER_API AEnemyTurret : public AEnemyBase
{
	GENERATED_BODY()

public:
	AEnemyTurret();

	virtual void BeginPlay() override;

	/// Accessors ///

	FORCEINLINE bool IsTargetingConstrained() { return ConstrainTargeting; }
	FORCEINLINE float GetRotationSpeed() { return RotationSpeed; }
	FORCEINLINE float GetFiringAngle() { return MinimumFiringAngle; }

protected:
	/// AI ///
	
	// If set to true, the turret will only fire orthagonal to its position, otherwise it will target the player in three dimensions
	UPROPERTY(Category = "AI", EditAnywhere)
		bool ConstrainTargeting;
	// The rate at which the turret can turn in degrees per second
	UPROPERTY(Category = "AI", EditAnywhere)
		float RotationSpeed;
	// The minimum angle between the turret and the player before the turret will fire
	UPROPERTY(Category = "AI", EditAnywhere)
		float MinimumFiringAngle;
	// If set to true the turret will attempt to track the player's movement when aiming
	UPROPERTY(Category = "AI", EditAnywhere)
		bool TrackPlayer;
};

UCLASS(BlueprintType, Blueprintable)
class CYBERSHOOTER_API AEnemyMovingTurret : public AEnemyTurret
{
	GENERATED_BODY()

public:
	AEnemyMovingTurret();

	virtual FVector GetVelocity() const override;

protected:
	/// Properties ///
	
	// The spline component that dictates the turret's patrol path
	UPROPERTY(Category = "Components", EditAnywhere)
		class USplineComponent* Spline;
	// The movement component for the turret
	UPROPERTY(Category = "Components", EditAnywhere)
		class USplineMovementComponent* MovementComponent;
};
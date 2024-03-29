// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

class ACyberShooterPlayer;
class AEnemyBase;

// Aiming settings for enemy AI
USTRUCT(BlueprintType)
struct FAimProfile
{
	GENERATED_BODY()

	// If set to true, the enemy will try to directi its aim at the player, otherwise it will rotate its aim at a constant speed based on AimSpeed
	UPROPERTY(EditAnywhere)
		bool TargetPlayer = true;
	// If set to true, the pawn will detect the player through walls
	UPROPERTY(EditAnywhere)
		bool SeeThroughWalls = false;
	// If set to true, the pawn will only fire perpendicular to its up vector, otherwise it will track the player in three dimensions, only used with TargetPlayer set to true
	UPROPERTY(EditAnywhere)
		bool ConstrainTargeting = true;

	// The speed at which the aiming vector rotates, in degrees per second
	UPROPERTY(EditAnywhere)
		float AimSpeed = 360.0f;
	// The maximum angle from which the pawn will shoot
	UPROPERTY(EditAnywhere)
		float FiringAngle = 5.0f;
};

// Movement settings for enemy AI
USTRUCT(BlueprintType)
struct FMovementProfile
{
	GENERATED_BODY()

	// The angle of movement relative to the player
	UPROPERTY(EditAnywhere)
		float Offset = 0.0f;
	// The relative speed of the pawn
	UPROPERTY(EditAnywhere)
		float Scale = 1.0f;
};

// The base controller for enemy AI
UCLASS(BlueprintType)
class CYBERSHOOTER_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	AEnemyAIController();

	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaSeconds) override;

	// Change the controller's AI settings
	UFUNCTION(BlueprintCallable)
		void SetAimProfile(const FAimProfile& Profile);
	UFUNCTION(BlueprintCallable)
		void SetMovementProfile(const FMovementProfile& Profile);

	// Start the AI
	void StartAI();
	// Stop the AI
	void StopAI();

	/// AI Functions ///
protected:
	// Determine whether or not the player is within the pawn's field of vision
	bool CheckLineOfSight();
	// Move the pawn's aiming component to track the player, returns the angle between the aim vector and the player
	float Aim(float DeltaTime);
	// Apply movement inputs to the pawn, returns the direction of movement
	FVector Move();

	/// Properties ///
public:
	// If set to true, the pawn will fire its weapon
	UPROPERTY(BlueprintReadWrite)
		bool FireWeapon;

protected:
	// Aiming settings
	UPROPERTY(EditAnywhere)
		FAimProfile AimSettings;
	// Movement settings
	UPROPERTY(EditAnywhere)
		FMovementProfile MoveSettings;

	// The pawn's current health, used to detect when the AI needs to change
	UPROPERTY(BlueprintReadOnly)
		int32 Health;
	// The pawn's current momentum, used to detect when the AI needs to change
	UPROPERTY(BlueprintReadOnly)
		int32 Momentum;
	// The pawn's current distance to the player, used to detect when the AI needs to change
	UPROPERTY(BlueprintReadOnly)
		int32 Distance;
	// The pawn's current aggro level, used to detect when the AI needs to change
	UPROPERTY(BlueprintReadOnly)
		int32 Aggro;

	// The player character
	UPROPERTY()
		ACyberShooterPlayer* Player;
	// The pawn being controlled by this AI
	UPROPERTY()
		AEnemyBase* EnemyPawn;
};
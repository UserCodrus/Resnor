// Copyright © 2020 Brian Faubion. All rights reserved.

#include "EnemyAIController.h"
#include "CyberShooterPlayer.h"
#include "CyberShooterEnemy.h"

#include "Kismet/GameplayStatics.h"

///
/// AEnemyAIController
///

AEnemyAIController::AEnemyAIController()
{
	Health = 0;
	Momentum = 0;
	Distance = 0;
	Aggro = 0;
}

void AEnemyAIController::BeginPlay()
{
	Super::BeginPlay();

	// Get a reference to the player and enemy
	Player = Cast<ACyberShooterPlayer>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	EnemyPawn = Cast<AEnemyBase>(GetPawn());
}

void AEnemyAIController::Tick(float DeltaSeconds)
{
	if (Active)
	{
		if (EnemyPawn != nullptr && Player != nullptr)
		{
			int32 dist = FVector::Distance(Player->GetActorLocation(), EnemyPawn->GetActorLocation()) / EnemyPawn->GetAIDistance();
			int32 blocks = EnemyPawn->GetMomentum() / EnemyPawn->GetMomentumBlockSize();

			// Detect when the AI might need to change
			bool health_change = (EnemyPawn->GetHealth() != Health);
			bool momentum_change = (blocks != Momentum);
			bool distance_change = (dist != Distance);
			bool aggro_change = (EnemyPawn->GetAggro() != Aggro);

			if (health_change || momentum_change || distance_change || aggro_change)
			{
				// Notify the pawn that its AI settings might need to change
				Health = EnemyPawn->GetHealth();
				Momentum = blocks;
				Distance = dist;
				Aggro = EnemyPawn->GetAggro();

				EnemyPawn->AIChange(this, health_change, momentum_change, distance_change, aggro_change);
			}
		}
	}
}

void AEnemyAIController::SetAimProfile(const FAimProfile& Profile)
{
	AimSettings = Profile;
}

void AEnemyAIController::SetMovementProfile(const FMovementProfile& Profile)
{
	MoveSettings = Profile;
}

/// AI Functions ///

bool AEnemyAIController::CheckLineOfSight()
{
	FCollisionQueryParams params;
	params.AddIgnoredActor(EnemyPawn);
	params.AddIgnoredActor(Player);

	FHitResult hit;
	return AimSettings.SeeThroughWalls || !GetWorld()->LineTraceSingleByChannel(hit, EnemyPawn->GetActorLocation(), Player->GetActorLocation(), ECollisionChannel::ECC_Visibility);
}

float AEnemyAIController::Aim(float DeltaTime)
{
	// Get an aiming vector in the enemy's frame of reference
	FRotator world_rotation = EnemyPawn->GetOrientationRotator();
	FVector aim = world_rotation.UnrotateVector(Player->GetActorLocation() - EnemyPawn->GetActorLocation());

	float angle = 180.0f;
	float max_angle = AimSettings.AimSpeed * DeltaTime;

	// Aim at the player
	if (AimSettings.ConstrainTargeting)
	{
		// Convert the aim to a 2D vector and interpolate is with the current rotation
		aim.Z = 0.0f;
		angle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(aim.GetUnsafeNormal(), EnemyPawn->GetAimLocalRotation().Vector())));
		if (angle > max_angle)
		{
			EnemyPawn->SetAimRotation(FQuat::Slerp(EnemyPawn->GetAimLocalRotation().Quaternion(), aim.Rotation().Quaternion(), max_angle / angle));
		}
		else
		{
			EnemyPawn->SetAimRotation(aim.Rotation());
		}
	}
	else
	{
		// Interpolate rotations to move the aim direction towards the player
		angle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(aim.GetUnsafeNormal(), EnemyPawn->GetAimLocalRotation().Vector())));
		if (angle > max_angle)
		{
			EnemyPawn->SetAimRotation(FQuat::Slerp(EnemyPawn->GetAimLocalRotation().Quaternion(), aim.Rotation().Quaternion(), max_angle / angle));
		}
		else
		{
			EnemyPawn->SetAimRotation(aim.Rotation());
		}
	}

	return angle;
}

FVector AEnemyAIController::Move()
{
	// Get a move vector in the pawn's local space and normalize it
	FVector direction = EnemyPawn->GetOrientationRotator().UnrotateVector(Player->GetActorLocation() - EnemyPawn->GetActorLocation());
	direction.Z = 0.0f;
	direction.Normalize();
	direction *= MoveSettings.Scale;

	// Rotate the move vector based on the move offset
	FRotator rotation(0.0f, MoveSettings.Offset, 0.0f);
	EnemyPawn->AddControlInput(rotation.RotateVector(direction));

	return direction;
}
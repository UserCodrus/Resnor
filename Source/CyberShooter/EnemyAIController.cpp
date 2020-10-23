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
	SeeThroughWalls = false;
	ConstrainTargeting = true;
	AimSpeed = 360.0f;
	FiringAngle = 10.0f;
}

void AEnemyAIController::BeginPlay()
{
	Super::BeginPlay();

	// Get a reference to the player and enemy
	Player = Cast<ACyberShooterPlayer>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	EnemyPawn = Cast<AEnemyBase>(GetPawn());
}

/// AI Functions ///

bool AEnemyAIController::CheckLineOfSight()
{
	FCollisionQueryParams params;
	params.AddIgnoredActor(EnemyPawn);
	params.AddIgnoredActor(Player);

	FHitResult hit;
	return SeeThroughWalls || !GetWorld()->LineTraceSingleByChannel(hit, EnemyPawn->GetActorLocation(), Player->GetActorLocation(), ECollisionChannel::ECC_Visibility);
}

float AEnemyAIController::Aim(float DeltaTime)
{
	// Get an aiming vector in the enemy's frame of reference
	FRotator world_rotation = EnemyPawn->GetOrientationRotator();
	FVector aim = world_rotation.UnrotateVector(Player->GetActorLocation() - EnemyPawn->GetActorLocation());

	float angle = 180.0f;
	float max_angle = AimSpeed * DeltaTime;

	// Aim at the player
	if (ConstrainTargeting)
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
	FVector direction = EnemyPawn->GetOrientationRotator().UnrotateVector(Player->GetActorLocation() - EnemyPawn->GetActorLocation());
	direction.Z = 0.0f;
	direction.Normalize();

	FRotator rotation(0.0f, MovementOffset, 0.0f);
	EnemyPawn->AddControlInput(rotation.RotateVector(direction));

	return direction;
}
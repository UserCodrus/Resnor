// Copyright © 2020 Brian Faubion. All rights reserved.

#include "EnemyTurretAI.h"
#include "CyberShooterEnemy.h"
#include "CyberShooterPlayer.h"

#include "Kismet/GameplayStatics.h"

AEnemyTurretAI::AEnemyTurretAI()
{
	EnemyPawn = nullptr;
}

void AEnemyTurretAI::BeginPlay()
{
	Super::BeginPlay();

	EnemyPawn = Cast<AEnemyTurret>(GetPawn());
	Player = Cast<ACyberShooterPlayer>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
}

void AEnemyTurretAI::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (EnemyPawn != nullptr && Player != nullptr)
	{
		// Determine if the player is within range of the turret
		FVector aim = Player->GetActorLocation() - EnemyPawn->GetActorLocation();
		if (aim.SizeSquared() < EnemyPawn->GetAggroDistance() * EnemyPawn->GetAggroDistance())
		{
			// Make sure the turret can see the player
			FCollisionQueryParams params;
			params.AddIgnoredActor(EnemyPawn);
			params.AddIgnoredActor(Player);

			FHitResult hit;
			if (!GetWorld()->LineTraceSingleByChannel(hit, EnemyPawn->GetActorLocation(), Player->GetActorLocation(), ECollisionChannel::ECC_Visibility))
			{
				// Convert the aim vector to the enemy's frame of reference
				FRotator world_rotation = EnemyPawn->GetOrientationRotator();
				aim = world_rotation.UnrotateVector(aim);

				float angle = 180.0f;
				float max_angle = EnemyPawn->GetRotationSpeed() * DeltaSeconds;

				// Aim the turret at the player
				if (EnemyPawn->IsTargetingConstrained())
				{
					// Convert the aim to a 2D vector and interpolate is with the current rotation
					aim.Z = 0.0f;
					angle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(aim.GetUnsafeNormal(), EnemyPawn->GetCoreLocalRotation().Vector())));
					if (angle > max_angle)
					{
						EnemyPawn->SetCoreRotation(FQuat::Slerp(EnemyPawn->GetCoreLocalRotation().Quaternion(), aim.Rotation().Quaternion(), max_angle / angle));
					}
					else
					{
						EnemyPawn->SetCoreRotation(aim.Rotation());
					}
				}
				else
				{
					// Interpolate rotations to move the enemy's aim towards the player
					angle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(aim.GetUnsafeNormal(), EnemyPawn->GetCoreLocalRotation().Vector())));
					if (angle > max_angle)
					{
						EnemyPawn->SetCoreRotation(FQuat::Slerp(EnemyPawn->GetCoreLocalRotation().Quaternion(), aim.Rotation().Quaternion(), max_angle / angle));
					}
					else
					{
						EnemyPawn->SetCoreRotation(aim.Rotation());
					}
				}

				// Fire the turret when it is close to aiming at the player
				if (angle <= EnemyPawn->GetFiringAngle())
				{
					EnemyPawn->StartFiring();
					return;
				}
			}
		}

		// Revert to no firing if any checks fail
		EnemyPawn->StopFiring();
	}
}
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
				float angle = 180.0f;
				float max_angle = EnemyPawn->GetRotationSpeed() * DeltaSeconds;

				// Aim the turret at the player
				if (EnemyPawn->IsTargetingConstrained())
				{
					// Convert the aim vector to a 2D vector in the enemy's frame of reference
					FRotator world_rotation = EnemyPawn->GetOrientationRotator();

					FVector local_aim = world_rotation.UnrotateVector(aim);
					Angle = FMath::RadiansToDegrees(local_aim.HeadingAngle());
					local_aim.Z = 0.0f;
					//local_aim = world_rotation.RotateVector(local_aim.GetSafeNormal());
					FVector current_aim = world_rotation.UnrotateVector(EnemyPawn->GetActorForwardVector());
					current_aim.Z = 0.0f;
					//current_aim = world_rotation.RotateVector(current_aim.GetSafeNormal());
					
					// Get the difference in angle between the aiming angle and the pawn's angle
					//float aim_angle = FMath::RadiansToDegrees(local_aim.HeadingAngle());
					//float pawn_angle = FMath::RadiansToDegrees(current_aim.HeadingAngle());
					//angle = aim_angle - pawn_angle;
					angle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(local_aim, current_aim)));
					
					//Angle = angle;
					//EnemyPawn->SetActorRotation(EnemyPawn->GetForwardVector().RotateAngleAxis(Angle, EnemyPawn->GetUpVector()).Rotation());
					//EnemyPawn->SetActorRotation(local_aim.Rotation());
					if (angle > max_angle)
					{
						//EnemyPawn->SetActorRotation(EnemyPawn->GetActorForwardVector().RotateAngleAxis(FMath::Sign(angle) * max_angle, EnemyPawn->GetUpVector()).Rotation());
						//EnemyPawn->SetActorRotation(FQuat::Slerp(current_aim.Rotation().Quaternion(), local_aim.Rotation().Quaternion(), max_angle / angle));
					}
					else
					{
						//EnemyPawn->SetActorRotation(EnemyPawn->GetActorForwardVector().RotateAngleAxis(angle, EnemyPawn->GetUpVector()).Rotation());
						//EnemyPawn->SetActorRotation(local_aim.Rotation());
					}
				}
				else
				{
					// Interpolate rotations to move the enemy's aim towards the player
					angle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(aim.GetUnsafeNormal(), EnemyPawn->GetActorForwardVector())));
					if (angle > max_angle)
					{
						EnemyPawn->SetActorRotation(FQuat::Slerp(EnemyPawn->GetActorRotation().Quaternion(), aim.Rotation().Quaternion(), max_angle / angle));
					}
					else
					{
						EnemyPawn->SetActorRotation(aim.Rotation());
					}
					Direction = aim;
					Angle = angle;
				}

				// Fire the turret when it is close to aiming at the player
				if (FMath::Abs(angle) <= EnemyPawn->GetFiringAngle())
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
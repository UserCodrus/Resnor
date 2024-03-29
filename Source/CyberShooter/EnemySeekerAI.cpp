// Copyright © 2020 Brian Faubion. All rights reserved.

#include "EnemySeekerAI.h"
#include "CyberShooterEnemy.h"
#include "CyberShooterPlayer.h"

#include "Kismet/GameplayStatics.h"

AEnemySeekerAI::AEnemySeekerAI()
{
	
}

void AEnemySeekerAI::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (EnemyPawn != nullptr && Player != nullptr)
	{
		FRotator world_rotation = EnemyPawn->GetOrientationRotator();

		// Make sure the turret can see the player
		if (CheckLineOfSight())
		{
			// Move towards the player
			FVector direction = Move();
				
			// Face the player
			FVector aim = world_rotation.RotateVector(direction);
			EnemyPawn->SetCoreRotation(direction.Rotation());

			// Fire the turret when it is close to aiming at the player
			if (FireWeapon && Aim(DeltaSeconds) <= AimSettings.FiringAngle)
			{
				EnemyPawn->StartFiring();
				return;
			}
		}

		// Revert to no firing if any checks fail
		EnemyPawn->StopFiring();
	}
}
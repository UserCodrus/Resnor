// Copyright © 2020 Brian Faubion. All rights reserved.

#include "EnemyTurretAI.h"
#include "CyberShooterEnemy.h"
#include "CyberShooterPlayer.h"

#include "Kismet/GameplayStatics.h"

AEnemyTurretAI::AEnemyTurretAI()
{
	
}

void AEnemyTurretAI::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (EnemyPawn != nullptr && Player != nullptr)
	{
		// Make sure the turret can see the player
		if (CheckLineOfSight())
		{
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
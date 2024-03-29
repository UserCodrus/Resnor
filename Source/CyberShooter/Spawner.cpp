// Copyright © 2020 Brian Faubion. All rights reserved.

#include "Spawner.h"

#include "CyberShooterEnemy.h"
#include "AggroZone.h"

#include "Engine/Engine.h"

ASpawner::ASpawner()
{
	SpawnComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SpawnComponent"));
	SpawnComponent->SetupAttachment(RootComponent);

	RequiredAggro = 1;
	MaxMinions = 5;
	SpawnTime = 5.0f;
	KillMinions = false;

	MinimumAggro = 0;
}

void ASpawner::SpawnChild()
{
	if (!Disabled && Minions.Num() < MaxMinions)
	{
		// Try to spawn an enemy
		FActorSpawnParameters spawn_params;
		spawn_params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::DontSpawnIfColliding;
		AEnemyMinion* enemy = GetWorld()->SpawnActor<AEnemyMinion>(EnemyType.Get(), SpawnComponent->GetComponentLocation(), SpawnComponent->GetComponentRotation(), spawn_params);

		// Claim ownership of the newly spawned enemy and set its aggro to match this spawner
		if (enemy != nullptr)
		{
			enemy->SpawnDefaultController();
			enemy->ClaimPawn(this);
			enemy->SetAggro(RequiredAggro, MinimumAggro);
			Minions.Push(enemy);
		}
	}

	// Restart the spawn timer
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_SpawnTimer, this, &ASpawner::SpawnChild, SpawnTime);
}

void ASpawner::KillChildren()
{
	for (int32 i = Minions.Num() - 1; i >= 0; --i)
	{
		Minions[i]->QuietKill();
	}
	Minions.Empty();
}

void ASpawner::NotifyDeath(AEnemyMinion* Child)
{
	Minions.Remove(Child);
}

/// ICombatInterface ///

void ASpawner::Kill()
{
	// Kill all the minions spawned by this spawner
	if (KillMinions)
	{
		KillChildren();
	}
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_SpawnTimer);

	// Notify parent zones that the spawner has despawned
	if (!Ephemeral)
	{
		for (int32 i = 0; i < ParentZone.Num(); ++i)
		{
			ParentZone[i]->NotifyDespawn();
		}
	}

	Super::Kill();
}

/// IAggroInterface ///

void ASpawner::Aggro()
{
	AggroLevel++;

	if (AggroLevel >= RequiredAggro)
	{
		// Start the spawn timer
		if (!GetWorld()->GetTimerManager().TimerExists(TimerHandle_SpawnTimer))
		{
			GetWorld()->GetTimerManager().SetTimer(TimerHandle_SpawnTimer, this, &ASpawner::SpawnChild, SpawnTime);
		}
	}

	// Aggro minions
	for (int32 i = 0; i < Minions.Num(); ++i)
	{
		Minions[i]->Aggro();
	}
}

void ASpawner::EndAggro()
{
	AggroLevel--;

	if (AggroLevel <= MinimumAggro)
	{
		// Cancel the timer
		if (GetWorld()->GetTimerManager().GetTimerRemaining(TimerHandle_SpawnTimer) > 0.0f)
		{
			GetWorld()->GetTimerManager().ClearTimer(TimerHandle_SpawnTimer);
		}
	}

	// Remove minion aggro
	for (int32 i = 0; i < Minions.Num(); ++i)
	{
		Minions[i]->EndAggro();
	}
}

void ASpawner::AggroReset()
{
	Super::AggroReset();

	KillChildren();
}

void ASpawner::RegisterZone(AAggroZone* Zone)
{
	if (!Ephemeral)
	{
		ParentZone.Add(Zone);
		Zone->NotifyRegister();
	}
}
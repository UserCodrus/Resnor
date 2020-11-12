// Copyright © 2020 Brian Faubion. All rights reserved.

#include "Destructible.h"

#include "Kismet/GameplayStatics.h"

ADestructible::ADestructible()
{
	// Needs to be movable to prevent shadows from baking
	RootComponent->SetMobility(EComponentMobility::Movable);

	MaxHealth = 10;
	Health = 0;
	Resistance = 0;
	
	RespawnCooldown = 2.0f;
	BlinkRate = 20;

	Disabled = false;
	Ephemeral = true;
	RespawnTimer = 0.0f;

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

void ADestructible::BeginPlay()
{
	Super::BeginPlay();

	Health = MaxHealth;
}

void ADestructible::Tick(float DeltaSeconds)
{
	if (RespawnTimer > 0.0f)
	{
		RespawnTimer -= DeltaSeconds;
		int state = (int)(RespawnTimer * BlinkRate) & 2;
		SetActorHiddenInGame((bool)state);
	}
	else
	{
		SetActorHiddenInGame(false);
		SetActorEnableCollision(true);
		SetActorTickEnabled(false);
	}
}

void ADestructible::Enable()
{
	Health = MaxHealth;

	// Start the respawn animation
	RespawnTimer = RespawnCooldown;
	SetActorTickEnabled(true);
	Disabled = false;
}

void ADestructible::Disable()
{
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	Disabled = true;
}

void ADestructible::Damage(int32 Value, int32 DamageType, AActor* Source, AActor* Origin)
{
	if (!(DamageImmunity & DamageType))
	{
		Value -= Resistance;
		if (Value > 0)
		{
			Health -= Value;

			if (Health <= 0.0f)
			{
				Kill();
			}
		}
	}
}

void ADestructible::Heal(int32 Value)
{
	if (Value > 0)
	{
		Health += Value;

		if (Health > MaxHealth)
		{
			Health = MaxHealth;
		}
	}
}

void ADestructible::Kill()
{
	// Create death particles
	if (DeathParticles != nullptr)
	{
		FTransform transform;
		transform.SetLocation(GetActorLocation());
		transform.SetRotation(GetActorRotation().Quaternion());
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DeathParticles, transform);
	}

	// Play the death sound
	if (DeathSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation());
	}

	if (Ephemeral)
	{
		Destroy();
	}
	else
	{
		Disable();
	}
}

/// IAggroInterface ///

void ADestructible::Aggro()
{
	// Not relevant
}

void ADestructible::EndAggro()
{
	// Not relevant
}

bool ADestructible::IsAggro()
{
	return false;
}

void ADestructible::AggroReset()
{
	Enable();
}
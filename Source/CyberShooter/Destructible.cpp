// Copyright © 2020 Brian Faubion. All rights reserved.

#include "Destructible.h"

#include "Kismet/GameplayStatics.h"

ADestructible::ADestructible()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent->SetMobility(EComponentMobility::Stationary);

	AggroLevel = 0;
	RequiredAggro = 0;

	MaxHealth = 10;
	Health = 0;
	Resistance = 0;
	
	RespawnCooldown = 2.0f;
	DamageCooldownDuration = 0.1f;
	BlinkRate = 20;

	Disabled = false;
	Ephemeral = true;

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
	if (DamageCooldown > 0.0f)
	{
		// Make the actor blink after taking damage
		DamageCooldown -= DeltaSeconds;
		int state = (int)(DamageCooldown * BlinkRate) & 2;
		SetActorHiddenInGame(!(bool)state);
	}
	else
	{
		// Show the actor if it isn't disabled and stop ticking
		SetActorHiddenInGame(Disabled);
		SetActorTickEnabled(false);
	}
}

void ADestructible::Enable()
{
	Health = MaxHealth;

	// Start the respawn animation
	DamageCooldown = RespawnCooldown;
	SetActorTickEnabled(true);
	SetActorEnableCollision(true);
	Disabled = false;
}

void ADestructible::Disable()
{
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	Disabled = true;
}

bool ADestructible::Damage(int32 Value, int32 DamageType, UForceFeedbackEffect* RumbleEffect, UPrimitiveComponent* HitComp, AActor* Source, AActor* Origin)
{
	if (!(DamageImmunity & DamageType) && DamageCooldown <= 0.0f && IsAggro())
	{
		Value -= Resistance;
		if (Value > 0)
		{
			Health -= Value;
			DamageCooldown = DamageCooldownDuration;
			SetActorTickEnabled(true);

			if (Health <= 0.0f)
			{
				Kill();
			}

			return true;
		}
	}

	return false;
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
	AggroLevel++;
}

void ADestructible::EndAggro()
{
	AggroLevel--;
}

bool ADestructible::IsAggro()
{
	return AggroLevel >= RequiredAggro;
}

void ADestructible::AggroReset()
{
	Enable();
}
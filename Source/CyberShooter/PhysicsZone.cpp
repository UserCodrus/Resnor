// Copyright © 2020 Brian Faubion. All rights reserved.

#include "PhysicsZone.h"
#include "CyberShooterProjectile.h"
#include "PhysicsInterface.h"

APhysicsZone::APhysicsZone()
{
	PrimaryActorTick.bCanEverTick = false;

	// Add overlap events
	OnActorBeginOverlap.AddDynamic(this, &APhysicsZone::BeginOverlap);
	OnActorEndOverlap.AddDynamic(this, &APhysicsZone::EndOverlap);

	// Set defaults
	AffectGravity = false;
	Gravity = 1000.0f;

	AffectFriction = false;
	Friction = 1.0f;
	AirFriction = 0.5f;

	Mass = 1.0f;
	Force = FVector(0.0f);
	TickSpeed = 1.0f;
}

void APhysicsZone::BeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	// Set object attributes
	IPhysicsInterface* object = Cast<IPhysicsInterface>(OtherActor);
	if (object != nullptr)
	{
		if (AffectGravity)
		{
			object->SetGravity(Gravity);
		}
		if (AffectFriction)
		{
			object->SetFriction(Friction);
			object->SetAirFriction(AirFriction);
		}
		if (Mass != 1.0f)
		{
			object->SetMass(Mass);
		}
		if (!Force.IsNearlyZero())
		{
			object->AddStaticForce(Force);
		}
		if (TickSpeed != 1.0f)
		{
			object->SetTickRate(TickSpeed);
		}
	}
	else
	{
		ACyberShooterProjectile* projectile = Cast<ACyberShooterProjectile>(OtherActor);
		if (projectile != nullptr)
		{
			if (AffectFriction)
			{
				projectile->SetAirFriction(AirFriction);
			}
			if (!Force.IsNearlyZero())
			{
				projectile->SetStaticForce(Force);
			}
			if (TickSpeed != 1.0f)
			{
				projectile->SetTickSpeed(TickSpeed);
			}
		}
	}
}

void APhysicsZone::EndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	// Reset object attributes
	IPhysicsInterface* object = Cast<IPhysicsInterface>(OtherActor);
	if (object != nullptr)
	{
		if (AffectGravity)
		{
			object->ResetGravity();
		}
		if (AffectFriction)
		{
			object->ResetFriction();
			object->ResetAirFriction();
		}
		if (Mass != 1.0f)
		{
			object->ResetMass();
		}
		if (!Force.IsNearlyZero())
		{
			object->ResetStaticForce();
		}
		if (TickSpeed != 1.0f)
		{
			object->SetTickRate(1.0f);
		}
	}
	else
	{
		ACyberShooterProjectile* projectile = Cast<ACyberShooterProjectile>(OtherActor);
		if (projectile != nullptr)
		{
			if (AffectFriction)
			{
				projectile->ResetAirFriction();
			}
			if (!Force.IsNearlyZero())
			{
				projectile->ResetStaticForce();
			}
			if (TickSpeed != 1.0f)
			{
				projectile->ResetTickSpeed();
			}
		}
	}
}
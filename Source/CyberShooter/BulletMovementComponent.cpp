// Copyright © 2020 Brian Faubion. All rights reserved.

#include "BulletMovementComponent.h"

UBulletMovementComponent::UBulletMovementComponent()
{
	// Set ProjectileMovementComponent settings
	InitialSpeed = 3000.0f;
	MaxSpeed = 12000.0f;

	bRotationFollowsVelocity = true;
	ProjectileGravityScale = 0.0f;

	bShouldBounce = true;
	Bounciness = 1.0f;
	Friction = 0.0f;

	// Set property defaults
	StaticForce = FVector(0.0f);
	AirFriction = 0.5f;
	TickSpeed = 1.0f;
	Acceleration = 0.0f;
}

void UBulletMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	// Change the tick rate
	DeltaTime *= TickSpeed;

	// Add static forces to the bullet's velocity
	Velocity += StaticForce * DeltaTime / AirFriction;

	// Apply acceleration
	if (Acceleration > 0.0f)
	{
		Velocity += Velocity.GetSafeNormal() * Acceleration * DeltaTime;
	}

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UBulletMovementComponent::AddVelocity(FVector VelocityImpulse)
{
	Velocity += VelocityImpulse;
}

void UBulletMovementComponent::SetTickSpeed(float NewSpeed)
{
	if (NewSpeed > 0.0f)
	{
		TickSpeed = NewSpeed;
	}
}

void UBulletMovementComponent::SetStaticForce(FVector NewForce)
{
	StaticForce = NewForce;
}

void UBulletMovementComponent::SetAirFriction(float NewFriction)
{
	if (NewFriction >= 0.0f)
	{
		AirFriction = NewFriction;
	}
}
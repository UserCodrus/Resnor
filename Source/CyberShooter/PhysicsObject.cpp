// Copyright © 2020 Brian Faubion. All rights reserved.

#include "PhysicsObject.h"
#include "PhysicsMovementComponent.h"
#include "CyberShooterGameInstance.h"

#include "Kismet/KismetMathLibrary.h"

APhysicsObject::APhysicsObject()
{
	// Create the movement component
	MovementComponent = CreateDefaultSubobject<UPhysicsMovementComponent>(TEXT("PhysicsMovementComponent"));
	MovementComponent->SetUpdatedComponent(RootComponent);

	// Make the mesh movable
	GetStaticMeshComponent()->Mobility = EComponentMobility::Movable;

	CanChangeOrientation = false;
	RespawnDistance = 10000.0f;

	PrimaryActorTick.bCanEverTick = true;
}

void APhysicsObject::BeginPlay()
{
	Super::BeginPlay();

	// Figure out how far to do floor checks
	FVector min, max;
	GetStaticMeshComponent()->GetLocalBounds(min, max);
	MovementComponent->Height = FMath::Abs(min.Z) * GetActorScale().Z;

	// Apply the current orientation to the movement component
	InitialForward = GetActorForwardVector();
	InitialUp = GetActorUpVector();
	MovementComponent->SetOrientation(GetActorForwardVector(), GetActorUpVector());

	RespawnPoint = GetActorLocation();

	// Set world physics
	ResetGravity();
	ResetAirFriction();
}

void APhysicsObject::Tick(float DeltaSeconds)
{
	// Manage respawning
	if (FVector::DistSquared(GetActorLocation(), RespawnPoint) > RespawnDistance * RespawnDistance)
	{
		MovementComponent->Teleport(RespawnPoint);
		SetOrientation(InitialForward, InitialUp);
	}
}

/// IPhysicsInterface ///

void APhysicsObject::AddImpulse(FVector Force)
{
	MovementComponent->AddImpulse(Force);
}

void APhysicsObject::AddRelativeImpulse(FVector Force)
{
	MovementComponent->AddImpulse(RootComponent->GetComponentRotation().RotateVector(Force));
}

void APhysicsObject::AddStaticForce(FVector Force)
{
	MovementComponent->SetStaticForce(Force);
}

void APhysicsObject::RemoveStaticForce(FVector Force)
{
	MovementComponent->SetStaticForce(-Force);
}

void APhysicsObject::ResetStaticForce()
{
	MovementComponent->ResetStaticForce();
}

void APhysicsObject::SetGravityEnabled(bool Enable)
{
	MovementComponent->GravityEnabled = Enable;
}

void APhysicsObject::SetGravity(float NewGravity)
{
	MovementComponent->Gravity = NewGravity;
}

void APhysicsObject::ResetGravity()
{
	UCyberShooterGameInstance* instance = Cast<UCyberShooterGameInstance>(GetWorld()->GetGameInstance());
	if (instance != nullptr)
	{
		MovementComponent->Gravity = instance->GetGravity();
	}
}

void APhysicsObject::SetAirFriction(float NewFriction)
{
	MovementComponent->AirFriction = NewFriction;
}

void APhysicsObject::ResetAirFriction()
{
	UCyberShooterGameInstance* instance = Cast<UCyberShooterGameInstance>(GetWorld()->GetGameInstance());
	if (instance != nullptr)
	{
		MovementComponent->AirFriction = instance->GetAirFriction();
	}
}

void APhysicsObject::SetFriction(float NewFriction)
{
	MovementComponent->Friction = NewFriction;
}

void APhysicsObject::ResetFriction()
{
	MovementComponent->Friction = 1.0f;
}

void APhysicsObject::SetTickRate(float NewRate)
{
	if (NewRate > 0.0f)
	{
		MovementComponent->SetTickSpeed(NewRate);
	}
}

/// IOrientationInterface ///

bool APhysicsObject::SetOrientation(FVector NewForward, FVector NewUp)
{
	if (CanChangeOrientation)
	{
		if (!NewForward.IsNearlyZero() && !NewUp.IsNearlyZero())
		{
			if (FVector::Orthogonal(NewForward, NewUp))
			{
				FRotator world_rotation = UKismetMathLibrary::MakeRotationFromAxes(NewForward, FVector::CrossProduct(NewUp, NewForward), NewUp);
				RootComponent->SetWorldRotation(world_rotation);

				MovementComponent->SetOrientation(GetActorForwardVector(), GetActorUpVector());

				return true;
			}
		}
	}

	return false;
}

bool APhysicsObject::CheckOrientation(FVector DesiredUp)
{
	return GetActorUpVector().Equals(DesiredUp.GetSafeNormal());
}
// Copyright © 2020 Brian Faubion. All rights reserved.

#include "PhysicsObject.h"
#include "PhysicsMovementComponent.h"

APhysicsObject::APhysicsObject()
{
	// Create the movement component
	MovementComponent = CreateDefaultSubobject<UPhysicsMovementComponent>(TEXT("PhysicsMovementComponent"));
	MovementComponent->SetUpdatedComponent(RootComponent);

	// Make the mesh movable
	GetStaticMeshComponent()->Mobility = EComponentMobility::Movable;
}

/// IBreakable Interface ///

void APhysicsObject::Impulse(FVector Force)
{
	MovementComponent->AddImpulse(Force);
}
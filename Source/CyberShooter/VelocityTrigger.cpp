// Copyright © 2020 Brian Faubion. All rights reserved.

#include "VelocityTrigger.h"
#include "PhysicsInterface.h"
#include "OrientationInterface.h"

#if WITH_EDITOR
#include "Components/ArrowComponent.h"
#endif

/// AVelocityTrigger ///

AVelocityTrigger::AVelocityTrigger()
{
	PrimaryActorTick.bCanEverTick = false;

	OnActorBeginOverlap.AddDynamic(this, &AVelocityTrigger::BeginOverlap);

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	DirectionComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Direction"));
	DirectionComponent->SetupAttachment(RootComponent);

#if WITH_EDITOR
	DirectionArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("DirectionArrow"));
	DirectionArrow->SetupAttachment(DirectionComponent);
	DirectionArrow->ArrowColor = FColor::Turquoise;
	DirectionArrow->bIsEditorOnly = true;

	OrientationArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("OrientationArrow"));
	OrientationArrow->SetupAttachment(RootComponent);
	OrientationArrow->SetRelativeLocation(FVector(0.0f, 25.0f, 0.0f));
	OrientationArrow->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));
	OrientationArrow->ArrowColor = FColor::Yellow;
	OrientationArrow->bIsEditorOnly = true;
#endif

	// Set defaults
	Magnitude = 1000.0f;
	RestrictOrientation = true;
}

void AVelocityTrigger::BeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	IOrientationInterface* orient_object = Cast<IOrientationInterface>(OtherActor);
	if (orient_object != nullptr)
	{
		// Make sure the player has the orientation required by the trigger
		if (RestrictOrientation)
		{
			if (!orient_object->CheckOrientation(RootComponent->GetUpVector()))
			{
				return;
			}
		}
	}

	IPhysicsInterface* physics_object = Cast<IPhysicsInterface>(OtherActor);
	if (physics_object != nullptr)
	{
		// Apply the velocity boost in the direction the component is facing
		physics_object->AddImpulse(DirectionComponent->GetForwardVector() * Magnitude);
	}
}
// Copyright © 2020 Brian Faubion. All rights reserved.

#include "OrientationTrigger.h"
#include "OrientationInterface.h"

#if WITH_EDITOR
#include "Components/ArrowComponent.h"
#endif

AOrientationTrigger::AOrientationTrigger()
{
	PrimaryActorTick.bCanEverTick = false;

	OnActorBeginOverlap.AddDynamic(this, &AOrientationTrigger::BeginOverlap);

	// Create components
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	OrientationComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Orientation"));
	OrientationComponent->SetUsingAbsoluteRotation(true);
	OrientationComponent->SetupAttachment(RootComponent);

	// Arrow components for editor visualizations
#if WITH_EDITOR
	OrientationUpArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("UpArrow"));
	OrientationUpArrow->SetupAttachment(OrientationComponent);
	OrientationUpArrow->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));
	OrientationUpArrow->ArrowColor = FColor::Blue;
	OrientationUpArrow->bIsEditorOnly = true;

	OrientationForwardArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("ForwardArrow"));
	OrientationForwardArrow->SetupAttachment(OrientationComponent);
	OrientationForwardArrow->ArrowColor = FColor::Red;
	OrientationForwardArrow->bIsEditorOnly = true;

	RequiredArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("RequiredArrow"));
	RequiredArrow->SetupAttachment(RootComponent);
	RequiredArrow->SetRelativeLocation(FVector(0.0f, 25.0f, 0.0f));
	RequiredArrow->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));
	RequiredArrow->ArrowColor = FColor::Yellow;
	RequiredArrow->bIsEditorOnly = true;
#endif

	// Set defaults
	Active = true;
	RestrictOrientation = true;
	ActiveWhenUnlocked = false;
}

/// ILockInterface ///

void AOrientationTrigger::Unlock()
{
	// Activate the trigger
	if (ActiveWhenUnlocked)
	{
		Active = true;
	}

	// Apply changes to manual targets
	for (int32 i = 0; i < Targets.Num(); ++i)
	{
		ChangeOrientation(Targets[i]);
	}

	Execute_OnUnlock(this);
}

void AOrientationTrigger::Lock()
{
	// Deactivate the trigger
	if (ActiveWhenUnlocked)
	{
		Active = false;
	}

	Execute_OnLock(this);
}

/// Orientation Trigger Functions ///

void AOrientationTrigger::BeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (Active)
	{
		ChangeOrientation(OtherActor);
	}
}

void AOrientationTrigger::ChangeOrientation(AActor* Target)
{
	IOrientationInterface* orient_object = Cast<IOrientationInterface>(Target);
	if (orient_object != nullptr)
	{
		// Skip the orientation change if the target is already in the proper orientation
		if (orient_object->CheckOrientation(OrientationComponent->GetUpVector()))
		{
			return;
		}

		// Make sure the target has the orientation required by the trigger
		if (RestrictOrientation)
		{
			if (!orient_object->CheckOrientation(RootComponent->GetUpVector()))
			{
				return;
			}
		}

		orient_object->SetOrientation(OrientationComponent->GetForwardVector(), OrientationComponent->GetUpVector());
	}
}
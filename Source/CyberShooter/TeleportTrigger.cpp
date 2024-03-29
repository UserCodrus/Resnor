// Copyright © 2020 Brian Faubion. All rights reserved.

#include "TeleportTrigger.h"
#include "OrientationInterface.h"
#include "CyberShooterPlayer.h"

#if WITH_EDITOR
#include "Components/ArrowComponent.h"
#endif

/// ATeleportBase ///

ATeleportBase::ATeleportBase()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent->bVisualizeComponent = true;

#if WITH_EDITOR
	OrientationUpArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("UpArrow"));
	OrientationUpArrow->SetupAttachment(RootComponent);
	OrientationUpArrow->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));
	OrientationUpArrow->ArrowColor = FColor::Blue;
	OrientationUpArrow->bIsEditorOnly = true;

	OrientationForwardArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("ForwardArrow"));
	OrientationForwardArrow->SetupAttachment(RootComponent);
	OrientationForwardArrow->ArrowColor = FColor::Red;
	OrientationForwardArrow->bIsEditorOnly = true;
#endif

	SetOrientation = true;
}

void ATeleportBase::OrientPlayer(IOrientationInterface* Player)
{
	if (SetOrientation)
	{
		// Set the player's orientation to match the component
		Player->SetOrientation(RootComponent->GetForwardVector(), RootComponent->GetUpVector());
	}
}

/// ATeleportTrigger ///

ATeleportTrigger::ATeleportTrigger()
{
	OnActorBeginOverlap.AddDynamic(this, &ATeleportTrigger::BeginOverlap);

	Active = true;
	Target = nullptr;
	RestrictOrientation = true;
}

void ATeleportTrigger::BeginPlay()
{
	// Execute lock commands
	if (!Active)
	{
		Deactivate();
	}
}

/// ILockInterface ///

void ATeleportTrigger::Activate()
{
	Active = true;
	ActivateTrigger();
}

void ATeleportTrigger::Deactivate()
{
	Active = false;
	DeactivateTrigger();
}

void ATeleportTrigger::BeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (Target == nullptr || !Active)
		return;

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

		ACyberShooterPlayer* player = Cast<ACyberShooterPlayer>(OtherActor);
		if (player != nullptr)
		{
			// Ensure that we didn't just get here from another telporter
			if (player->CheckTeleport(this))
			{
				// Teleport the player
				player->Teleport(this, Target);
			}
		}
	}
}
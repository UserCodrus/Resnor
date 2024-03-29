// Copyright © 2020 Brian Faubion. All rights reserved.

#include "DamageTrigger.h"
#include "CombatInterface.h"
#include "PhysicsInterface.h"

#if WITH_EDITOR
#include "Components/ArrowComponent.h"
#endif

ADamageTrigger::ADamageTrigger()
{
	PrimaryActorTick.bCanEverTick = true;

	OnActorBeginOverlap.AddDynamic(this, &ADamageTrigger::BeginOverlap);
	OnActorEndOverlap.AddDynamic(this, &ADamageTrigger::EndOverlap);

	// Create the direction component
	DirectionComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Direction"));
	DirectionComponent->SetUsingAbsoluteRotation(true);
	DirectionComponent->SetupAttachment(RootComponent);

	// Arrow components for editor visualizations
#if WITH_EDITOR
	DirectionArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("DirectionArrow"));
	DirectionArrow->SetupAttachment(DirectionComponent);
	DirectionArrow->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));
	DirectionArrow->SetUsingAbsoluteScale(true);
	DirectionArrow->ArrowColor = FColor::Turquoise;
	DirectionArrow->bIsEditorOnly = true;
#endif

	// Set property defaults
	Damage = 1;
	
	Disabled = false;
	AutoCycle = true;
	Active = false;
	Impulse = 0.0f;
	InstantDamage = false;
	TargetOverlap = true;
	ActiveDuration = 1.0f;
	InactiveDuration = 1.0f;
	TimerOffset = 0.0f;
}


void ADamageTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Disabled)
		return;

	if (AutoCycle)
	{
		// Increment the activity timer
		Timer += DeltaTime;
		if (Active)
		{
			if (Timer > ActiveDuration)
			{
				// Deactivate the trigger
				DeactivateTrigger();
				Timer -= ActiveDuration;
			}
		}
		else
		{
			if (Timer > InactiveDuration)
			{
				// Activate the trigger
				ActivateTrigger();
				Timer -= InactiveDuration;
			}
		}
	}

	// Deal damage to each object inside the trigger
	if (!InstantDamage && Active)
	{
		ApplyDamage();
	}
}

void ADamageTrigger::BeginPlay()
{
	Super::BeginPlay();

	// Set the default timer
	Timer = TimerOffset;
}

/// Damage Trigger Functions ///

void ADamageTrigger::BeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (!TargetOverlap)
		return;

	// Add actors to the target list
	ICombatInterface* object = Cast<ICombatInterface>(OtherActor);
	if (object != nullptr)
	{
		Targets.Add(OtherActor);
	}

	// Apply damage once
	if (InstantDamage && !AutoCycle)
	{
		ApplyDamage();
		Targets.Remove(OtherActor);
	}
}

void ADamageTrigger::EndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (!TargetOverlap)
		return;

	// Remove actors from the target list
	ICombatInterface* object = Cast<ICombatInterface>(OtherActor);
	if (object != nullptr)
	{
		Targets.Remove(OtherActor);
	}
}

void ADamageTrigger::ApplyDamage()
{
	// Damage each target
	for (int32 i = 0; i < Targets.Num(); ++i)
	{
		ICombatInterface* object = Cast<ICombatInterface>(Targets[i]);
		if (object != nullptr)
		{
			object->Damage(Damage, EDamageType::DAMAGETYPE_ENVIRONMENT, RumbleEffect, nullptr, this, this);

			// Apply physics impulse if needed
			if (Impulse != 0.0f)
			{
				IPhysicsInterface* physics_object = Cast<IPhysicsInterface>(object);
				if (physics_object != nullptr)
				{
					physics_object->ChangeVelocity(FVector::ZeroVector);
					physics_object->AddImpulse(DirectionComponent->GetUpVector() * Impulse);
				}
			}
		}
	}
}

void ADamageTrigger::ActivateTrigger()
{
	Active = true;
	if (InstantDamage)
	{
		// Deal damage and cycle the timer
		ApplyDamage();
		Timer += ActiveDuration;
	}

	Activate();
}

void ADamageTrigger::DeactivateTrigger()
{
	Active = false;

	Deactivate();
}

void ADamageTrigger::DisableTrigger()
{
	Disabled = true;

	DeactivateTrigger();
}

void ADamageTrigger::EnableTrigger()
{
	Disabled = false;

	Timer = 0.0f;
	ActivateTrigger();
}
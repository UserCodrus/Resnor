// Copyright © 2020 Brian Faubion. All rights reserved.

#include "SplineMovementComponent.h"

#include "Components/SplineComponent.h"

USplineMovementComponent::USplineMovementComponent()
{
	// Set defaults
	Spline = nullptr;
	DelayTime = 1.0f;
	RoundTrip = true;

	Timer = 0.0f;
	Delayed = true;
	ReturnTrip = true;
}

void USplineMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (Spline == nullptr)
		return;

	// Manage the movement timer
	if (Timer > 0.0f)
	{
		Timer -= DeltaTime;
	}
	if (Timer <= 0.0f)
	{
		// Start or stop movement when the timer is up
		if (Delayed)
		{
			StartMoving();
		}
		else
		{
			StopMoving();
		}
	}

	if (!Delayed)
	{
		// Change the position of the object over time
		if (ReturnTrip)
		{
			UpdatedComponent->SetWorldLocation(Spline->GetWorldLocationAtTime(Timer, true));
		}
		else
		{
			UpdatedComponent->SetWorldLocation(Spline->GetWorldLocationAtTime(Spline->Duration - Timer, true));
		}
	}
	else
	{
		// Set the position to a static location
		if (ReturnTrip)
		{
			UpdatedComponent->SetWorldLocation(Spline->GetWorldLocationAtSplinePoint(0));
		}
		else
		{
			UpdatedComponent->SetWorldLocation(Spline->GetWorldLocationAtSplinePoint(Spline->GetNumberOfSplinePoints() - 1));
		}
	}
}

void USplineMovementComponent::StartMoving()
{
	Timer = Spline->Duration;
	Delayed = false;

	if (RoundTrip)
	{
		ReturnTrip = !ReturnTrip;
	}
}

void USplineMovementComponent::StopMoving()
{
	Timer = DelayTime;
	Delayed = true;
}
// Copyright © 2020 Brian Faubion. All rights reserved.

#include "SplineMovementComponent.h"

#include "Components/SplineComponent.h"

USplineMovementComponent::USplineMovementComponent()
{
	// Set defaults
	CanMove = true;
	Spline = nullptr;

	SafeMovement = false;
	Automated = true;
	RoundTrip = true;
	Delay = 1.0f;
	OneWay = false;
	Reverse = true;

	Timer = 0.0f;
	IsDelayed = true;
}

void USplineMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (!CanMove || Spline == nullptr)
		return;

	// Manage the movement timer
	if (Timer > 0.0f)
	{
		Timer -= DeltaTime;
	}
	if (Timer <= 0.0f)
	{
		// Start or stop movement when the timer is up
		if (IsDelayed)
		{
			if (Automated)
			{
				// Always move when automated
				StartMoving();
			}
			else if (RoundTrip && !Reverse)
			{
				// Return to origin if round trip is enabled and we are at the destination
				StartMoving();
			}
		}
		else
		{
			StopMoving();
		}
	}

	// Calculate the actor's movement
	FVector start_location = UpdatedComponent->GetComponentLocation();
	FQuat rotation = UpdatedComponent->GetComponentQuat();
	FVector delta = FVector::ZeroVector;
	if (!IsDelayed)
	{
		// Change the current position
		if (Reverse)
		{
			
			delta = Spline->GetWorldLocationAtTime(Timer, true) - UpdatedComponent->GetComponentLocation();
			//UpdatedComponent->SetWorldLocation(Spline->GetWorldLocationAtTime(Timer, true));
		}
		else
		{
			delta = Spline->GetWorldLocationAtTime(Spline->Duration - Timer, true) - UpdatedComponent->GetComponentLocation();
			//UpdatedComponent->SetWorldLocation(Spline->GetWorldLocationAtTime(Spline->Duration - Timer, true));
		}
	}
	else
	{
		// Set the position to a static location
		if (Reverse)
		{
			delta = Spline->GetWorldLocationAtSplinePoint(0) - UpdatedComponent->GetComponentLocation();
		}
		else
		{
			delta = Spline->GetWorldLocationAtSplinePoint(Spline->GetNumberOfSplinePoints() - 1) - UpdatedComponent->GetComponentLocation();
		}
	}

	// Move the actor
	if (!delta.IsNearlyZero())
	{
		if (SafeMovement)
		{
			FHitResult hit(1.0f);
			SafeMoveUpdatedComponent(delta, rotation, true, hit);

			// Handle collisions
			if (hit.IsValidBlockingHit())
			{
				HandleImpact(hit, DeltaTime, delta);
				SlideAlongSurface(delta, 1.0f - hit.Time, hit.Normal, hit, true);
			}
		}
		else
		{
			MoveUpdatedComponent(delta, rotation, false);
		}
	}

	// Register the actor's current velocity based on actual movement
	Velocity = (UpdatedComponent->GetComponentLocation() - start_location) / DeltaTime;
	UpdateComponentVelocity();
}

bool USplineMovementComponent::StartMovement()
{
	if (IsDelayed)
	{
		StartMoving();
		return true;
	}
	else
	{
		return false;
	}
}

void USplineMovementComponent::StartMoving()
{
	Timer = Spline->Duration;
	IsDelayed = false;

	if (!OneWay)
	{
		Reverse = !Reverse;
	}
}

void USplineMovementComponent::StopMoving()
{
	Timer = Delay;
	IsDelayed = true;
}
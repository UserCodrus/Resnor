// Copyright © 2020 Brian Faubion. All rights reserved.

#include "SplineMovementComponent.h"

#include "Components/SplineComponent.h"

USplineMovementComponent::USplineMovementComponent()
{
	// Set defaults
	CanMove = true;
	Spline = nullptr;

	FullPath = true;
	SafeMovement = false;
	Automated = true;
	RoundTrip = true;
	Delay = 1.0f;
	OneWay = false;
	Reverse = false;

	Timer = 0.0f;
	IsDelayed = true;
	CurrentPoint = 0;
}

void USplineMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	if (FullPath)
	{
		if (!OneWay)
		{
			// Invert reverse since it will be changed on the first frame by StartMoving()
			Reverse = !Reverse;
		}
	}
	else
	{
		if (!OneWay)
		{
			Reverse = !Reverse;
		}

		// Set the current point so we begin at the start or end of the spline based on the reverse setting when StartMoving() is called on the first frame
		if (Reverse)
		{
			CurrentPoint = 1;
		}
		else
		{
			CurrentPoint = Spline->GetNumberOfSplinePoints() - 2;
		}
	}

	if (Reverse)
	{

	}
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
		if (FullPath)
		{
			if (Reverse)
			{
				delta = Spline->GetWorldLocationAtTime(Timer, true) - UpdatedComponent->GetComponentLocation();
			}
			else
			{
				delta = Spline->GetWorldLocationAtTime(Spline->Duration - Timer, true) - UpdatedComponent->GetComponentLocation();
			}
		}
		else
		{
			// Get the distance to travel
			FVector distance;
			if (Reverse)
			{
				distance = Spline->GetWorldLocationAtSplinePoint(CurrentPoint - 1) - Spline->GetWorldLocationAtSplinePoint(CurrentPoint);
			}
			else
			{
				distance = Spline->GetWorldLocationAtSplinePoint(CurrentPoint + 1) - Spline->GetWorldLocationAtSplinePoint(CurrentPoint);
			}
			distance = distance * DeltaTime / Spline->Duration;

			delta = distance;
		}
	}
	else
	{
		// Set the position to a static location
		if (FullPath)
		{
			if (Reverse)
			{
				delta = Spline->GetWorldLocationAtSplinePoint(0) - UpdatedComponent->GetComponentLocation();
			}
			else
			{
				delta = Spline->GetWorldLocationAtSplinePoint(Spline->GetNumberOfSplinePoints() - 1) - UpdatedComponent->GetComponentLocation();
			}
		}
		else
		{
			if (Reverse)
			{
				delta = Spline->GetWorldLocationAtSplinePoint(CurrentPoint - 1) - UpdatedComponent->GetComponentLocation();
			}
			else
			{
				delta = Spline->GetWorldLocationAtSplinePoint(CurrentPoint + 1) - UpdatedComponent->GetComponentLocation();
			}
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

void USplineMovementComponent::MoveToStart()
{
	if (Timer > 0.0f)
	{
		if (IsDelayed)
		{
			// The object is stopped, move it if we are not at the start
			if (!Reverse)
			{
				StartMoving();
			}
		}
		else
		{
			// The object is moving to the end, reverse it
			if (!Reverse)
			{
				Reverse = !Reverse;
				Timer = Spline->Duration - Timer;
			}
		}
	}
	else
	{
		// Start moving if we are not already at the start
		if (!Reverse)
		{
			StartMoving();
		}
	}
}

void USplineMovementComponent::MoveToEnd()
{
	if (Timer > 0.0f)
	{
		if (IsDelayed)
		{
			// The object is stopped, move it if we are not at the end
			if (Reverse)
			{
				StartMoving();
			}
		}
		else
		{
			// The object is moving to the start, reverse it
			if (Reverse)
			{
				Reverse = !Reverse;
				Timer = Spline->Duration - Timer;
			}
		}
	}
	else
	{
		// Start moving if we are not already at the end
		if (Reverse)
		{
			StartMoving();
		}
	}
}

void USplineMovementComponent::StartMoving()
{
	Timer = Spline->Duration;
	IsDelayed = false;

	if (FullPath)
	{
		// Reverse direction
		if (!OneWay)
		{
			Reverse = !Reverse;
		}
	}
	else
	{
		// Set the new endpoint
		if (!OneWay)
		{
			if (Reverse)
			{
				CurrentPoint--;
				if (CurrentPoint == 0)
				{
					// Reverse at the end of the spline
					Reverse = false;
				}
			}
			else
			{
				CurrentPoint++;
				if (CurrentPoint == Spline->GetNumberOfSplinePoints() - 1)
				{
					// Reverse at the end of the spline
					Reverse = true;
				}
			}
		}
		else
		{
			if (Reverse)
			{
				CurrentPoint--;
				if (CurrentPoint == 0)
				{
					// Loop at the end of the spline
					CurrentPoint = Spline->GetNumberOfSplinePoints() - 1;
				}
			}
			else
			{
				CurrentPoint++;
				if (CurrentPoint == Spline->GetNumberOfSplinePoints() - 1)
				{
					// Loop at the end of the spline
					CurrentPoint = 0;
				}
			}
		}
	}
}

void USplineMovementComponent::StopMoving()
{
	Timer = Delay;
	IsDelayed = true;
}
// Copyright © 2020 Brian Faubion. All rights reserved.

#include "PlayerMovementComponent.h"

#include "Engine/Engine.h"

UPlayerMovementComponent::UPlayerMovementComponent()
{
	MaxSpeed = 1000.0f;
	Acceleration = 2000.0f;
	Deceleration = 4000.0f;

	TurningMultiplier = 4.0f;
	SpeedMultiplier = 1.0f;

	MaxIncline = 45.0f;
	Up = FVector(0.0f, 0.0f, 1.0f);
	Gravity= 500.0f;

	FaceVelocity = false;
}

void UPlayerMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	// Ignore the tick if the component can't move
	if (ShouldSkipUpdate(DeltaTime))
	{
		return;
	}

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	const AController* controller = PawnOwner->GetController();
	if (controller && controller->IsLocalController())
	{
		// Apply controller input to velocity
		if (controller->IsLocalPlayerController() == true)
		{
			ApplyControlInputToVelocity(DeltaTime);
		}

		// Get the current velocity and move the component
		FVector delta = Velocity * DeltaTime;
		if (!delta.IsNearlyZero())
		{
			FVector oldlocation = UpdatedComponent->GetComponentLocation();
			FQuat rotation = UpdatedComponent->GetComponentQuat();

			// Move the component
			FHitResult hit(1.0f);
			SafeMoveUpdatedComponent(delta, rotation, true, hit);

			// Handle collisions
			if (hit.IsValidBlockingHit())
			{
				HandleImpact(hit, DeltaTime, delta);

				// Calculate the normal of a slope with the maximum angle
				FVector base_step(0.0f, 0.0f, 1.0f);
				FRotator step_delta(MaxIncline, 0.0f, 0.0f);

				// Ensure that the hit normal isn't steeper than the max slope
				FVector new_normal = hit.Normal;
				if (FMath::Abs(hit.Normal.Z) < step_delta.RotateVector(base_step).Z)
				{
					new_normal = hit.Normal.GetSafeNormal2D();
				}
				SlideAlongSurface(delta, 1.0f - hit.Time, new_normal, hit, true);
			}

			// Update velocity so that position corrections don't cause problems
			FVector newlocation = UpdatedComponent->GetComponentLocation();
			Velocity = ((newlocation - oldlocation) / DeltaTime);

			// Rotate the pawn to match velocity
			if (FaceVelocity)
			{
				UpdatedComponent->SetWorldRotation(Velocity.Rotation());
			}
		}

		// Finalize the update
		UpdateComponentVelocity();
	}
}

void UPlayerMovementComponent::ChangeSpeed(float NewSpeed)
{
	if (NewSpeed > 0.0f)
	{
		SpeedMultiplier = NewSpeed;
	}
}

void UPlayerMovementComponent::ApplyControlInputToVelocity(float DeltaTime)
{
	// Get the current input vector
	FVector current_acceleration = GetPendingInputVector().GetClampedToMaxSize(1.0f);

	// The magnitude of the input vector
	float analog_modifier = (current_acceleration.SizeSquared() > 0.0f ? current_acceleration.Size() : 0.0f);
	// The max speed of the pawn relative to the input vector
	float current_max_speed = MaxSpeed * analog_modifier * SpeedMultiplier;
	// Determine if we are exceeding the current max speed
	bool exceed_max = IsExceedingMaxSpeed(current_max_speed);
	
	if (analog_modifier > 0.0f && !exceed_max)
	{
		// Apply changes in direction
		if (Velocity.SizeSquared() > 0.0f)
		{
			float turn_scale = FMath::Clamp(DeltaTime * TurningMultiplier, 0.0f, 1.0f);
			Velocity = Velocity + (current_acceleration * Velocity.Size() - Velocity) * turn_scale;
		}
	}
	else
	{
		// Decelerate
		if (Velocity.SizeSquared() > 0.0f)
		{
			FVector oldvelocity = Velocity;
			float magnitude = FMath::Max(Velocity.Size() - FMath::Abs(Deceleration * SpeedMultiplier) * DeltaTime, 0.f);
			Velocity = Velocity.GetSafeNormal() * magnitude;

			// Don't brake lower than the max speed
			if (exceed_max && Velocity.SizeSquared() < FMath::Square(current_max_speed))
			{
				Velocity = oldvelocity.GetSafeNormal() * current_max_speed;
			}
		}
	}

	// Apply current_acceleration
	float newspeed = (IsExceedingMaxSpeed(current_max_speed)) ? Velocity.Size() : current_max_speed;
	Velocity += current_acceleration * FMath::Abs(Acceleration * SpeedMultiplier) * DeltaTime;
	Velocity = Velocity.GetClampedToMaxSize(newspeed);

	ConsumeInputVector();
}
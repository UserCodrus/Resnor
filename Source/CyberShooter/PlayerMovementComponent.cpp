// Copyright © 2020 Brian Faubion. All rights reserved.

#include "PlayerMovementComponent.h"
#include "PhysicalStaticMesh.h"

#include "Components/CapsuleComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Kismet/KismetMathLibrary.h"

UPlayerMovementComponent::UPlayerMovementComponent()
{
	MaxSpeed = 1000.0f;
	Acceleration = 2000.0f;
	Deceleration = 4000.0f;

	TurningRate = 5.0f;
	SpeedMultiplier = 1.0f;

	Up = FVector(0.0f, 0.0f, 1.0f);
	Forward = FVector(1.0f, 0.0f, 0.0f);
	KeepMomentum = true;

	StaticForce = FVector(0.0f);
	Gravity= 1000.0f;
	GravityEnabled = true;
	AirFriction = 0.5f;
	Friction = 1.0f;
	DefaultSurfaceFriction = 1.0f;
	MaxFloorDistance = 5.0f;
	TerminalVelocity = 5000.0f;
	TickSpeed = 1.0f;

	PendingImpulse = FVector(0.0f);

	Teleported = false;
}

void UPlayerMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	// Ignore the tick if the component can't move
	if (ShouldSkipUpdate(DeltaTime))
	{
		return;
	}

	DeltaTime *= TickSpeed;

	// Reset velocity after being teleported
	if (Teleported)
	{
		Teleported = false;
		Velocity = FVector(0.0f, 0.0f, 0.0f);
	}

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	const AController* controller = PawnOwner->GetController();
	if (controller && controller->IsLocalController())
	{
		// Apply controller input to velocity
		ApplyControlInputToVelocity(DeltaTime);

		// Apply gravity
		ApplyGravityToVelocity(DeltaTime);

		// Apply impulses and static forces to velocity
		FRotator orientation = UKismetMathLibrary::MakeRotationFromAxes(Forward, FVector::CrossProduct(Up, Forward), Up);
		Velocity += orientation.RotateVector(PendingImpulse);
		PendingImpulse = FVector(0.0f);

		// Get the current velocity
		FVector delta = orientation.RotateVector(Velocity * DeltaTime);
		if (CurrentFloor.Walkable)
		{
			delta += CurrentFloor.Velocity * DeltaTime;
		}
		delta += StaticForce * DeltaTime / GetFriction();

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
				/*FVector base_step(0.0f, 0.0f, 1.0f);
				FRotator step_delta(MaxIncline, 0.0f, 0.0f);

				// Ensure that the hit normal isn't steeper than the max slope
				FVector new_normal = hit.Normal;
				if (FMath::Abs(hit.Normal.Z) < step_delta.RotateVector(base_step).Z)
				{
					new_normal = hit.Normal.GetSafeNormal2D();
				}*/
				SlideAlongSurface(delta, 1.0f - hit.Time, hit.Normal, hit, true);
			}

			// Update velocity so that position corrections don't cause problems
			//FVector newlocation = UpdatedComponent->GetComponentLocation();
			//Velocity = ((newlocation - oldlocation) / DeltaTime);
		}

		// Finalize the update
		UpdateComponentVelocity();
	}
}

void UPlayerMovementComponent::OnTeleported()
{
	Super::OnTeleported();

	// Mark the pawn as teleported so that velocity can be reset
	Teleported = true;
}

void UPlayerMovementComponent::SetSpeed(float NewSpeed)
{
	if (NewSpeed > 0.0f)
	{
		SpeedMultiplier = NewSpeed;
	}
}

void UPlayerMovementComponent::SetGravity(float NewGravity)
{
	Gravity = NewGravity;
}

void UPlayerMovementComponent::EnableGravity(bool Enable)
{
	GravityEnabled = Enable;
}

void UPlayerMovementComponent::SetAirFriction(float NewFriction)
{
	AirFriction = NewFriction;
}

void UPlayerMovementComponent::SetFriction(float NewFriction)
{
	Friction = NewFriction;
}

void UPlayerMovementComponent::SetCollision(UCapsuleComponent* NewCollision)
{
	Collision = NewCollision;
}

void UPlayerMovementComponent::SetOrientation(FVector NewForward, FVector NewUp)
{
	// Normalize the vectors
	NewForward = NewForward.GetSafeNormal();
	NewUp = NewUp.GetSafeNormal();

	// Make sure the vectors are orthogonal
	if (!NewForward.IsNearlyZero() && !NewUp.IsNearlyZero())
	{
		if (FVector::Orthogonal(NewForward, NewUp))
		{
			// Convert velocity into new orientation
			if (KeepMomentum)
			{
				FVector world_velocity = UKismetMathLibrary::MakeRotationFromAxes(Forward, FVector::CrossProduct(Up, Forward), Up).RotateVector(Velocity);
				Velocity = UKismetMathLibrary::MakeRotationFromAxes(NewForward, FVector::CrossProduct(NewUp, NewForward), NewUp).RotateVector(world_velocity);
			}

			Forward = NewForward;
			Up = NewUp;
		}
	}
}

void UPlayerMovementComponent::SetStaticForce(FVector Force)
{
	StaticForce = Force;
}

void UPlayerMovementComponent::ResetStaticForce()
{
	PendingImpulse += StaticForce / GetFriction();
	StaticForce = FVector(0.0f);
}

void UPlayerMovementComponent::AddImpulse(FVector Impulse)
{
	PendingImpulse += Impulse;
}

void UPlayerMovementComponent::StopGravity()
{
	Velocity.Z = 0.0f;
}

void UPlayerMovementComponent::SetTickSpeed(float NewSpeed)
{
	if (NewSpeed > 0.0f)
	{
		TickSpeed = NewSpeed;
	}
}

bool UPlayerMovementComponent::IsFalling() const
{
	return !CurrentFloor.Walkable;
}

float UPlayerMovementComponent::GetFloorDamage() const
{
	return CurrentFloor.Damage;
}

void UPlayerMovementComponent::ApplyControlInputToVelocity(float DeltaTime)
{
	// Get the current input vector
	FVector current_acceleration = GetPendingInputVector().GetClampedToMaxSize(1.0f);
	// Get the horizontal velocity so we can calculate velocity ignoring gravity
	FVector relative_velocity(Velocity.X, Velocity.Y, 0.0f);

	// Get the input vector magnitude
	float analog_modifier = (current_acceleration.SizeSquared() > 0.0f ? current_acceleration.Size() : 0.0f);
	// The max speed of the pawn relative to the input vector
	float current_max_speed = GetMaxSpeed() * analog_modifier;
	// Determine if we are exceeding the current max speed
	bool exceed_max = relative_velocity.SizeSquared() > FMath::Square(current_max_speed);
	
	if (!exceed_max)
	{
		// Apply changes in direction
		if (relative_velocity.SizeSquared() > 0.0f)
		{
			float turn_scale = FMath::Clamp(DeltaTime * GetTurnRate(), 0.0f, 1.0f);
			relative_velocity += (current_acceleration * relative_velocity.Size() - relative_velocity) * turn_scale;
		}

		// Apply acceleration
		relative_velocity += current_acceleration * GetAcceleration() * DeltaTime;

		// Cap max speed
		if (relative_velocity.SizeSquared() > FMath::Square(current_max_speed))
		{
			relative_velocity = relative_velocity.GetSafeNormal() * current_max_speed;
		}
	}
	else
	{
		if (relative_velocity.SizeSquared() > 0.0f)
		{
			// Apply deceleration
			float magnitude = FMath::Max(relative_velocity.Size() - GetDeceleration() * DeltaTime, 0.0f);
			relative_velocity = relative_velocity.GetSafeNormal() * magnitude;

			// Don't allow deceleration to drop us below max speed
			/*if (relative_velocity.SizeSquared() < FMath::Square(current_max_speed))
			{
				relative_velocity = relative_velocity.GetSafeNormal() * current_max_speed;
			}*/
		}
	}

	// Apply changes to velocity
	relative_velocity.Z = Velocity.Z;
	Velocity = relative_velocity;

	ConsumeInputVector();
}

void UPlayerMovementComponent::ApplyGravityToVelocity(float DeltaTime)
{
	if (GravityEnabled)
	{
		// Find the floor below the object
		FFloor newfloor;
		FindFloor(newfloor);

		// Check for velocity changes when moving off of objects
		if (CurrentFloor.Velocity != newfloor.Velocity)
		{
			AddImpulse(CurrentFloor.Velocity - newfloor.Velocity);
		}

		CurrentFloor = newfloor;

		// Apply gravity
		if (!CurrentFloor.Walkable)
		{
			if (Velocity.Z > GetTerminalVelocity())
			{
				Velocity.Z -= GetGravity() * DeltaTime;
			}
			else
			{
				Velocity.Z = GetTerminalVelocity();
			}
		}
		else
		{
			Velocity.Z = 0.0f;
		}
	}
	else
	{
		// Ignore the floor when gravity is disabled
		FFloor nofloor;
		nofloor.Friction = AirFriction;
		CurrentFloor = nofloor;

		// Decelerate vertically at the same rate as horizontal movement
		if (Velocity.Z != 0.0f)
		{
			float direction = 1.0f;
			if (Velocity.Z < 0.0f)
			{
				direction = -1.0f;
			}
			Velocity.Z = direction * FMath::Max(FMath::Abs(Velocity.Z) - GetDeceleration() * DeltaTime, 0.0f);
		}
	}
}

bool UPlayerMovementComponent::FindFloor(FFloor& Result)
{
	if (Collision == nullptr)
		return false;

	// Get the size of the collision capsule
	float radius, height;
	Collision->GetScaledCapsuleSize(radius, height);

	// Set collision parameters
	FCollisionQueryParams params;
	FCollisionResponseParams response;
	params.AddIgnoredComponent(Cast<UPrimitiveComponent>(UpdatedComponent));
	InitCollisionParams(params, response);
	ECollisionChannel channel = UpdatedComponent->GetCollisionObjectType();

	// Perform a line trace
	float shrink_height = height;
	float trace_distance = MaxFloorDistance + shrink_height;
	FVector trace_start = UpdatedComponent->GetComponentLocation();
	FVector trace_end = trace_start + Up * -trace_distance;
	bool block = GetWorld()->LineTraceSingleByChannel(Result.HitResult, trace_start, trace_end, channel, params, response);

	if (block)
	{
		if (Result.HitResult.Time > 0.0f)
		{
			// Calculate the distance to the hit
			Result.Distance = FMath::Max(-radius, Result.HitResult.Time * trace_distance - shrink_height);
			if (Result.Distance <= MaxFloorDistance)
			{
				// Mark the floor as walkable
				Result.Walkable = true;

				// Set velocity for moving platforms
				if (Result.HitResult.Actor != nullptr)
				{
					Result.Velocity = Result.HitResult.Actor->GetVelocity();
				}

				// Set the friction from the mesh if it is a physics mesh
				APhysicalStaticMesh* mesh = Cast<APhysicalStaticMesh>(Result.HitResult.Actor);
				if (mesh != nullptr)
				{
					Result.Friction = mesh->GetFriction();
					Result.Damage = mesh->GetDamage();
				}
				else
				{
					Result.Friction = DefaultSurfaceFriction;
				}
			}
			else
			{
				// Use air friction if the floor is too far away
				Result.Friction = AirFriction;
			}
		}
	}
	else
	{
		// Use air friction if nothing is below us
		Result.Friction = AirFriction;
	}

	return block;
}

float UPlayerMovementComponent::GetMaxSpeed() const
{
	return MaxSpeed * SpeedMultiplier;
}

float UPlayerMovementComponent::GetAcceleration() const
{
	return Acceleration * SpeedMultiplier * GetFriction();
}

float UPlayerMovementComponent::GetDeceleration() const
{
	return Deceleration * SpeedMultiplier * GetFriction() * GetFriction();
}

float UPlayerMovementComponent::GetTurnRate() const
{
	return TurningRate * GetFriction() * GetFriction();
}

float UPlayerMovementComponent::GetFriction() const
{
	return CurrentFloor.Friction * Friction;
}

float UPlayerMovementComponent::GetGravity() const
{
	return Gravity;
}

float UPlayerMovementComponent::GetTerminalVelocity() const
{
	return -TerminalVelocity;
}
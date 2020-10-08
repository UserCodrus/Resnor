// Copyright © 2020 Brian Faubion. All rights reserved.

#include "PhysicsMovementComponent.h"
#include "PhysicalStaticMesh.h"

#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"

///
/// UPhysicsMovementComponent
///

UPhysicsMovementComponent::UPhysicsMovementComponent()
{
	Awake = false;

	MaxSpeed = 1000.0f;
	Acceleration = 2000.0f;
	Deceleration = 2000.0f;

	TurningRate = 4.0f;
	SpeedMultiplier = 1.0f;

	Up = FVector(0.0f, 0.0f, 1.0f);
	Forward = FVector(1.0f, 0.0f, 0.0f);
	KeepMomentum = true;
	Height = 0.0f;

	StaticForce = FVector(0.0f);
	Gravity= 1000.0f;
	GravityEnabled = true;
	AirFriction = 0.5f;
	Friction = 1.0f;
	SurfaceFriction = 1.0f;
	MaxFloorDistance = 10.0f;
	MinFloorDistance = 5.0f;
	TerminalVelocity = 5000.0f;
	TickSpeed = 1.0f;

	PendingInput = FVector::ZeroVector;
	PendingImpulse = FVector::ZeroVector;
	PendingTeleport = FVector::ZeroVector;

	TeleportReady = false;
}

void UPhysicsMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	// Skip the tick if the object is not awake
	if (!Awake)
		return;

	// Ignore the tick if the component can't move
	if (ShouldSkipUpdate(DeltaTime))
	{
		return;
	}

	DeltaTime *= TickSpeed;

	// Apply gravity
	ApplyGravityToVelocity(DeltaTime);

	// Apply impulses and static forces to velocity
	ApplyImpulseToVelocity();

	// Decelerate the component
	ApplyDeceleration(DeltaTime);

	// Get the current velocity
	FVector delta = GetTotalVelocity() * DeltaTime;

	if (!delta.IsNearlyZero())
	{
		MoveComponent(delta, DeltaTime);
	}
	else
	{
		// Put the object to sleep
		if (IsStable())
		{
			Awake = false;
		}
	}

	// Finalize the update
	UpdateComponentVelocity();
}

/// Physics ///

void UPhysicsMovementComponent::Teleport(FVector Location)
{
	Awake = true;
	Velocity = FVector::ZeroVector;

	TeleportReady = true;
	PendingTeleport = Location;
}

void UPhysicsMovementComponent::SetSpeed(float NewSpeed)
{
	if (NewSpeed > 0.0f)
	{
		SpeedMultiplier = NewSpeed;
	}
}

void UPhysicsMovementComponent::SetOrientation(FVector NewForward, FVector NewUp)
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
				Velocity = UKismetMathLibrary::MakeRotationFromAxes(NewForward, FVector::CrossProduct(NewUp, NewForward), NewUp).UnrotateVector(world_velocity);
			}

			Forward = NewForward;
			Up = NewUp;
		}
	}
}

void UPhysicsMovementComponent::AddControlInput(FVector Input)
{
	Awake = true;
	PendingInput += Input;
}

void UPhysicsMovementComponent::SetStaticForce(FVector Force)
{
	Awake = true;
	StaticForce = Force;
}

void UPhysicsMovementComponent::ResetStaticForce()
{
	StaticForce = FVector(0.0f);
}

void UPhysicsMovementComponent::AddImpulse(FVector Impulse)
{
	Awake = true;
	PendingImpulse += Impulse;
}

void UPhysicsMovementComponent::StopGravity()
{
	Velocity.Z = 0.0f;
}

void UPhysicsMovementComponent::StopMovement()
{
	Velocity = FVector(0.0f);
}

void UPhysicsMovementComponent::SetTickSpeed(float NewSpeed)
{
	if (NewSpeed > 0.0f)
	{
		TickSpeed = NewSpeed;
	}
}

FVector UPhysicsMovementComponent::GetTotalVelocity() const
{
	// Base velocity translated to world space
	FVector total = UpdatedComponent->GetComponentQuat().RotateVector(Velocity);

	// Velocity from the object the pawn is standing on
	if (CurrentFloor.Walkable)
	{
		total += CurrentFloor.Velocity;
	}

	// Static force modifier
	total += GetStaticForce();

	return total;
}

bool UPhysicsMovementComponent::IsFalling() const
{
	return !CurrentFloor.Walkable;
}

bool UPhysicsMovementComponent::IsStable() const
{
	return CurrentFloor.Stable && (CurrentFloor.Damage == 0.0f);
}

float UPhysicsMovementComponent::GetFloorDamage() const
{
	return CurrentFloor.Damage;
}

/// Movement ///

void UPhysicsMovementComponent::MoveComponent(FVector Delta, float DeltaTime)
{
	if (TeleportReady)
	{
		// Teleport the component to its new location
		FHitResult hit(1.0f);
		SafeMoveUpdatedComponent(PendingTeleport - UpdatedComponent->GetComponentLocation(), UpdatedComponent->GetComponentQuat(), false, hit, ETeleportType::ResetPhysics);

		Velocity = FVector::ZeroVector;
		TeleportReady = false;
	}
	else
	{
		FVector oldlocation = UpdatedComponent->GetComponentLocation();
		FQuat rotation = UpdatedComponent->GetComponentQuat();

		// Move the component
		FHitResult hit(1.0f);
		SafeMoveUpdatedComponent(Delta, rotation, true, hit);

		// Handle collisions
		if (hit.IsValidBlockingHit())
		{
			HandleImpact(hit, DeltaTime, Delta);

			//Velocity = Velocity.MirrorByVector(UpdatedComponent->GetComponentQuat().RotateVector(HitNormal));

			// Calculate the normal of a slope with the maximum angle
			/*FVector base_step(0.0f, 0.0f, 1.0f);
			FRotator step_delta(MaxIncline, 0.0f, 0.0f);

			// Ensure that the hit normal isn't steeper than the max slope
			FVector new_normal = hit.Normal;
			if (FMath::Abs(hit.Normal.Z) < step_delta.RotateVector(base_step).Z)
			{
				new_normal = hit.Normal.GetSafeNormal2D();
			}*/
			SlideAlongSurface(Delta, 1.0f - hit.Time, hit.Normal, hit, true);
		}

		// Update velocity to account for position corrections and impacts
		FVector newvelocity = (UpdatedComponent->GetComponentLocation() - oldlocation) / DeltaTime;
		newvelocity -= GetStaticForce();
		if (CurrentFloor.Walkable)
		{
			newvelocity -= CurrentFloor.Velocity;
		}

		Velocity = UpdatedComponent->GetComponentQuat().UnrotateVector(newvelocity);
	}
}

void UPhysicsMovementComponent::ApplyDeceleration(float DeltaTime)
{
	// Reduce the velocity based on deceleration
	FVector relative_velocity(Velocity.X, Velocity.Y, 0.0f);
	if (relative_velocity.SizeSquared() > 0.0f)
	{
		float magnitude = FMath::Max(relative_velocity.Size() - GetDeceleration() * DeltaTime, 0.0f);
		relative_velocity = relative_velocity.GetSafeNormal() * magnitude;
	}

	// Apply the reduction
	relative_velocity.Z = Velocity.Z;
	Velocity = relative_velocity;
}

void UPhysicsMovementComponent::ApplyImpulseToVelocity()
{
	FQuat orientation = UpdatedComponent->GetComponentQuat();
	Velocity += orientation.UnrotateVector(PendingImpulse);
	PendingImpulse = FVector(0.0f);
}

void UPhysicsMovementComponent::ApplyGravityToVelocity(float DeltaTime)
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
			// Cancel gravity
			if (Velocity.Z < 0.0f)
			{
				Velocity.Z = 0.0f;
			}

			// Snap to the floor to help moving platforms work better
			UpdatedComponent->SetWorldLocation(UpdatedComponent->GetComponentLocation() + Up * (MinFloorDistance - CurrentFloor.Distance));
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

bool UPhysicsMovementComponent::FindFloor(FFloor& Result)
{
	// Set collision parameters
	FCollisionQueryParams params;
	FCollisionResponseParams response;
	params.AddIgnoredActor(UpdatedComponent->GetAttachmentRootActor());
	InitCollisionParams(params, response);
	ECollisionChannel channel = UpdatedComponent->GetCollisionObjectType();

	// Perform a line trace
	float shrink_height = Height;
	float trace_distance = MaxFloorDistance + shrink_height;
	FVector trace_start = UpdatedComponent->GetComponentLocation();
	FVector trace_end = trace_start + Up * -trace_distance;
	bool block = GetWorld()->LineTraceSingleByChannel(Result.HitResult, trace_start, trace_end, channel, params, response);

	if (block)
	{
		if (Result.HitResult.Time > 0.0f)
		{
			// Calculate the distance to the hit
			Result.Distance = FMath::Max(-Height, Result.HitResult.Time * trace_distance - shrink_height);
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
					Result.Stable = mesh->IsStable();
				}
				else
				{
					Result.Friction = SurfaceFriction;
					if (Result.HitResult.Actor != nullptr)
					{
						Result.Stable = !Result.HitResult.Actor->IsRootComponentMovable();
					}
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

/// Properties ///

float UPhysicsMovementComponent::GetMaxSpeed() const
{
	return MaxSpeed * SpeedMultiplier;
}

float UPhysicsMovementComponent::GetAcceleration() const
{
	return Acceleration * SpeedMultiplier * GetFriction();
}

float UPhysicsMovementComponent::GetDeceleration() const
{
	return Deceleration * SpeedMultiplier * GetFriction() * GetFriction();
}

float UPhysicsMovementComponent::GetTurnRate() const
{
	return TurningRate * GetFriction() * GetFriction();
}

float UPhysicsMovementComponent::GetFriction() const
{
	return CurrentFloor.Friction * Friction;
}

float UPhysicsMovementComponent::GetGravity() const
{
	return Gravity;
}

FVector UPhysicsMovementComponent::GetStaticForce() const
{
	return StaticForce / GetFriction();
}

float UPhysicsMovementComponent::GetTerminalVelocity() const
{
	return -TerminalVelocity;
}

///
/// UPlayerMovementComponent
///

UPlayerMovementComponent::UPlayerMovementComponent()
{
	Deceleration = 4000.0f;
}

void UPlayerMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	// Skip the tick if the object is not awake
	if (!Awake)
		return;

	// Ignore the tick if the component can't move
	if (ShouldSkipUpdate(DeltaTime))
	{
		return;
	}

	DeltaTime *= TickSpeed;

	// Apply control inputs
	ApplyControlInputToVelocity(DeltaTime);

	// Apply gravity
	ApplyGravityToVelocity(DeltaTime);

	// Apply impulses and static forces to velocity
	ApplyImpulseToVelocity();

	// Get the current velocity
	FVector delta = GetTotalVelocity() * DeltaTime;

	if (!delta.IsNearlyZero())
	{
		MoveComponent(delta, DeltaTime);
	}

	// Finalize the update
	UpdateComponentVelocity();
}

/// Movement ///

void UPlayerMovementComponent::ApplyControlInputToVelocity(float DeltaTime)
{
	// Get the current input vector
	FVector current_acceleration = PendingInput.GetClampedToMaxSize(1.0f);
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

	PendingInput = FVector::ZeroVector;
}
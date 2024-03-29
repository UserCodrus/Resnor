// Copyright © 2020 Brian Faubion. All rights reserved.

#include "PhysicsMovementComponent.h"
#include "PhysicalStaticMesh.h"
#include "PhysicsInterface.h"

#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"

///
/// UPhysicsMovementComponent
///

UPhysicsMovementComponent::UPhysicsMovementComponent()
{
	Disabled = false;

	MaxSpeed = 1000.0f;
	Acceleration = 2000.0f;
	Deceleration = 2000.0f;

	TurningRate = 4.0f;
	SpeedMultiplier = 1.0f;

	KeepMomentum = true;
	Height = 0.0f;

	StaticForce = FVector(0.0f);
	Mass = 1.0f;
	Gravity= 1000.0f;
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
	// Ignore the tick if the component can't move
	if (ShouldSkipUpdate(DeltaTime))
	{
		return;
	}

	// Stop movement and sleep when disabled
	if (Disabled)
	{
		Velocity = FVector::ZeroVector;
		PendingInput = FVector::ZeroVector;
		PendingImpulse = FVector::ZeroVector;
		Sleep();
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

	if (!delta.IsNearlyZero() || TeleportReady)
	{
		MoveComponent(delta, DeltaTime);
	}
	else
	{
		// Put the object to sleep
		if (IsStable())
		{
			Sleep();
		}
	}

	// Finalize the update
	UpdateComponentVelocity();
}

/// Physics ///

void UPhysicsMovementComponent::SetVelocity(FVector NewVelocity)
{
	Wake();

	// Subtract static force and floor velocity to get the relative velocity
	NewVelocity -= GetStaticForce();
	if (CurrentFloor.Walkable)
	{
		NewVelocity -= CurrentFloor.Velocity;
	}

	// Convert world space velocity to local space
	Velocity = UpdatedComponent->GetComponentQuat().UnrotateVector(NewVelocity);
}

void UPhysicsMovementComponent::Impact(IPhysicsInterface* OtherObject, FVector ImpactNormal)
{
	if (!Disabled && OtherObject->CanMove())
	{
		// Make sure this object is the one causing the impact
		if (FVector::DotProduct(GetTotalVelocity().GetSafeNormal(), ImpactNormal) < 0.0f)
		{
			Wake();
			FVector v1 = GetTotalVelocity();
			FVector v2 = OtherObject->GetVelocity();

			// Calculate velocity along the normal of the impact
			float a1 = FVector::DotProduct(v1, ImpactNormal);
			float a2 = FVector::DotProduct(v2, ImpactNormal);

			// Calculate the change in momentum
			float deltap = (2.0f * (a1 - a2)) / (Mass + OtherObject->GetMass());

			// Set the new velocity of the objects
			SetVelocity(v1 - deltap * OtherObject->GetMass() * ImpactNormal);
			OtherObject->ChangeVelocity(v2 + deltap * Mass * ImpactNormal);
		}
	}
}

void UPhysicsMovementComponent::Teleport(FVector Location)
{
	Wake();
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

void UPhysicsMovementComponent::AddControlInput(FVector Input)
{
	Wake();
	PendingInput += Input;
}

void UPhysicsMovementComponent::SetStaticForce(FVector Force)
{
	Wake();
	StaticForce = Force;
}

void UPhysicsMovementComponent::ResetStaticForce()
{
	StaticForce = FVector(0.0f);
}

void UPhysicsMovementComponent::AddImpulse(FVector Impulse)
{
	Wake();
	PendingImpulse += Impulse / Mass;
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

void UPhysicsMovementComponent::Wake()
{
	if (!Disabled)
	{
		SetComponentTickEnabled(true);
	}
}

void UPhysicsMovementComponent::Sleep()
{
	SetComponentTickEnabled(false);
}

void UPhysicsMovementComponent::Enable()
{
	Disabled = false;
	Wake();
}

void UPhysicsMovementComponent::Disable()
{
	Disabled = true;
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
		SetVelocity((UpdatedComponent->GetComponentLocation() - oldlocation) / DeltaTime);
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
	if (Gravity != 0.0f)
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
			if (Velocity.Z <= 0.0f)
			{
				Velocity.Z = 0.0f;

				// Snap to the floor to help moving platforms work better
				UpdatedComponent->SetWorldLocation(UpdatedComponent->GetComponentLocation() + UpdatedComponent->GetUpVector() * (MinFloorDistance - CurrentFloor.Distance));
			}
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
	InitCollisionParams(params, response);
	params.AddIgnoredActor(UpdatedComponent->GetAttachmentRootActor());
	ECollisionChannel channel = UpdatedComponent->GetCollisionObjectType();

	// Perform a line trace
	float shrink_height = Height;
	float trace_distance = MaxFloorDistance + shrink_height;
	FVector trace_start = UpdatedComponent->GetComponentLocation();
	FVector trace_end = trace_start + UpdatedComponent->GetUpVector() * -trace_distance;
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
	return Acceleration * SpeedMultiplier * GetFriction() / Mass;
}

float UPhysicsMovementComponent::GetDeceleration() const
{
	return Deceleration * SpeedMultiplier * GetFriction() * GetFriction() / Mass;
}

float UPhysicsMovementComponent::GetTurnRate() const
{
	return TurningRate * GetFriction() * GetFriction() / Mass;
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
	return StaticForce / (GetFriction() * Mass);
}

FVector UPhysicsMovementComponent::GetForce() const
{
	return GetTotalVelocity() * Mass;
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

	if (!delta.IsNearlyZero() || TeleportReady)
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
	FVector current_acceleration = PendingInput.GetClampedToMaxSize2D(1.0f);
	current_acceleration.Z = 0.0f;
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

///
/// UEnemyMovementComponent
///

UEnemyMovementComponent::UEnemyMovementComponent()
{

}

void UEnemyMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	// Ignore the tick if the component can't move
	if (ShouldSkipUpdate(DeltaTime))
	{
		return;
	}

	// Stop movement and sleep when disabled
	if (Disabled)
	{
		Velocity = FVector::ZeroVector;
		PendingInput = FVector::ZeroVector;
		PendingImpulse = FVector::ZeroVector;
		Sleep();
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

	if (!delta.IsNearlyZero() || TeleportReady)
	{
		MoveComponent(delta, DeltaTime);
	}
	else
	{
		// Put the enemy to sleep
		if (IsStable())
		{
			Sleep();
		}
	}

	// Finalize the update
	UpdateComponentVelocity();
}
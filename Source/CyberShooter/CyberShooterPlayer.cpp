// Copyright © 2020 Brian Faubion. All rights reserved.

#include "CyberShooterPlayer.h"
#include "PhysicsMovementComponent.h"
#include "CyberShooterPlayerController.h"
#include "CyberShooterGameInstance.h"
#include "TeleportTrigger.h"
#include "Weapon.h"
#include "Ability.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"

#include "Engine/Engine.h"

const FName ACyberShooterPlayer::MoveForwardBinding("MoveForward");
const FName ACyberShooterPlayer::MoveRightBinding("MoveRight");

ACyberShooterPlayer::ACyberShooterPlayer()
{
	// Set up a fixed camera
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("PlayerSpringArmComponent"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->SetAbsolute(true, true, false);
	SpringArmComponent->TargetArmLength = 1200.f;
	SpringArmComponent->bDoCollisionTest = false;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("PlayerTopDownCamera"));
	CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
	CameraComponent->SetUsingAbsoluteRotation(true);

	MovementComponent = CreateDefaultSubobject<UPlayerMovementComponent>(TEXT("PlayerMovementComponent"));
	MovementComponent->SetUpdatedComponent(RootComponent);

	CollisionComponent->OnComponentHit.AddDynamic(this, &ACyberShooterPlayer::OnHit);

	// Set properties
	Momentum = 0.0f;
	MomentumBonus = 0.1f;
	MomentumReward = 0.0f;
	MomentumBlockSize = 20.0f;

	SpeedMultiplier = 1.0f;
	GravityMultiplier = 1.0f;
	Mass = 1.0f;
	RespawnDistance = 10000.0f;
	DamageImmunity = DAMAGETYPE_PLAYER;
	CollisionForce = 1.5f;

	CameraAngle = FRotator(-75.0f, 0.0f, 0.0f);
	CameraTransitionTime = 1.0f;

	// Set other values
	RespawnPoints.SetNum(3);
	CurrentRespawn = 0;

	LastTeleportTarget = nullptr;
	CameraStartRotation = FQuat();
	CameraDesiredRotation = FQuat();
	CameraStartPosition = FVector(0.0f);
	CameraDesiredPosition = FVector(0.0f);
	TransitionPosition = false;
	CameraTimer = 0.0f;
	FloorDamageTimer = 0.0f;
}

/// APawn Functions ///

void ACyberShooterPlayer::BeginPlay()
{
	Super::BeginPlay();

	// Set up the movement component
	MovementComponent->Height = CollisionComponent->GetScaledCapsuleHalfHeight();
	ApplyOrientation(GetForwardVector(), GetUpVector(), true);

	// Set world physics
	ResetGravity();
	ResetAirFriction();
	ResetMass();

	// Set respawn points
	for (int32 i = 0; i < RespawnPoints.Num(); ++i)
	{
		RespawnPoints[i].Location = GetActorLocation();
		RespawnPoints[i].Forward = GetForwardVector();
		RespawnPoints[i].Up = GetUpVector();
	}
}

void ACyberShooterPlayer::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Set tick speed
	DeltaSeconds *= TickSpeed;

	// Get the current rotation of the pawn relative to the up and forward vectors
	FRotator world_rotation = GetOrientationRotator();

	// Handle camera transitions
	if (CameraTimer > 0.0f)
	{
		CameraTransition(DeltaSeconds);
	}
	else
	{
		SpringArmComponent->SetWorldLocation(GetActorLocation());
	}
	
	// Adjust movement attributes
	MovementComponent->SetSpeed((1.0f + Momentum / MomentumBlockSize * MomentumBonus) * SpeedMultiplier);

	// Apply movement inputs
	FVector move_direction = FVector(GetInputAxisValue(MoveForwardBinding), GetInputAxisValue(MoveRightBinding), 0.0f).GetClampedToMaxSize(1.0f);
	if (!move_direction.IsZero())
	{
		MovementComponent->AddControlInput(move_direction);
	}

	// Rotate to face the movement direction
	if (move_direction.SizeSquared() > 0.0f)
	{
		CoreComponent->SetRelativeRotation(move_direction.Rotation());
	}

	// Try to fire a shot if the radial menus aren't open and the player is shooting
	ACyberShooterPlayerController* controller = Cast<ACyberShooterPlayerController>(GetController());
	if (controller != nullptr && !controller->IsMenuOpen() && FireWeapon)
	{
		// Fire in the direction the player is aiming or in the direction the pawn is facing
		FVector fire_direction = world_rotation.RotateVector(controller->GetFireDirection());
		if (fire_direction.IsNearlyZero())
		{
			fire_direction = CoreComponent->GetComponentRotation().Vector();
		}
		Fire(fire_direction);
	}

	// Take damage from environmental hazards
	if (MovementComponent->GetFloorDamage() > 0.0f || EnvironmentDamage > 0.0f)
	{
		FloorDamageTimer += (MovementComponent->GetFloorDamage() + EnvironmentDamage) * DeltaSeconds;

		if (FloorDamageTimer >= 1.0f)
		{
			int32 damage = FMath::TruncToInt(FloorDamageTimer);
			Damage(damage, DAMAGETYPE_ENVIRONMENT, nullptr, nullptr);
			FloorDamageTimer -= (float)damage;

			if (EnvironmentRumble != nullptr)
			{
				if (controller != nullptr)
				{
					controller->ClientPlayForceFeedback(EnvironmentRumble);
				}
			}
		}
	}
	else
	{
		FloorDamageTimer = 0.0f;
	}

	// Manage respawns
	if (IsFalling())
	{
		// Respawn the player after they have fallen too far
		if (FVector::DistSquared(GetActorLocation(), GetRespawn().Location) > RespawnDistance * RespawnDistance)
		{
			MovementComponent->Teleport(GetRespawn().Location);
			ApplyOrientation(GetRespawn().Forward, GetRespawn().Up, true);
			Damage(1, DAMAGETYPE_ENVIRONMENT);

			if (FallRumble != nullptr)
			{
				if (controller != nullptr)
				{
					controller->ClientPlayForceFeedback(FallRumble);
				}
			}
		}
	}
	else
	{
		// Set the player's respawn point
		if (IsStable())
		{
			SetRespawn();
		}
	}
}

void ACyberShooterPlayer::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	// Set up gameplay key bindings
	PlayerInputComponent->BindAxis(MoveForwardBinding);
	PlayerInputComponent->BindAxis(MoveRightBinding);

	PlayerInputComponent->BindAction("Fire", EInputEvent::IE_Pressed, this, &ACyberShooterPlayer::StartFiring);
	PlayerInputComponent->BindAction("Fire", EInputEvent::IE_Released, this, &ACyberShooterPlayer::StopFiring);
	PlayerInputComponent->BindAction("Ability", EInputEvent::IE_Pressed, this, &ACyberShooterPlayer::StartAbility);
	PlayerInputComponent->BindAction("Ability", EInputEvent::IE_Released, this, &ACyberShooterPlayer::StopAbility);
}

FVector ACyberShooterPlayer::GetVelocity() const
{
	return MovementComponent->GetTotalVelocity();
}

void ACyberShooterPlayer::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor != nullptr && OtherActor != this)
	{
		IPhysicsInterface* object = Cast<IPhysicsInterface>(OtherActor);
		if (object != nullptr)
		{
			// Handle the collision
			MovementComponent->Impact(object, Hit.Normal);

			// Add controller rumble
			if (ImpactRumble != nullptr)
			{
				APlayerController* controller = Cast<APlayerController>(GetController());
				if (controller != nullptr)
				{
					controller->ClientPlayForceFeedback(ImpactRumble);
				}
			}
		}
	}
}

/// ICombatInterface ///

void ACyberShooterPlayer::Kill()
{
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, "Player has been killed");
}

/// IPhysicsInterface ///


FVector ACyberShooterPlayer::GetVelocity()
{
	return MovementComponent->GetTotalVelocity();
}

void ACyberShooterPlayer::ChangeVelocity(FVector NewVelocity)
{
	MovementComponent->SetVelocity(NewVelocity);
}

void ACyberShooterPlayer::AddImpulse(FVector Force)
{
	MovementComponent->AddImpulse(Force);
}

void ACyberShooterPlayer::AddRelativeImpulse(FVector Force)
{
	MovementComponent->AddImpulse(CoreComponent->GetComponentRotation().RotateVector(Force));
}

void ACyberShooterPlayer::AddStaticForce(FVector Force)
{
	MovementComponent->SetStaticForce(Force);
}

void ACyberShooterPlayer::RemoveStaticForce(FVector Force)
{
	MovementComponent->SetStaticForce(-Force);
}

void ACyberShooterPlayer::ResetStaticForce()
{
	MovementComponent->ResetStaticForce();
}

void ACyberShooterPlayer::SetGravity(float NewGravity)
{
	WorldGravity = NewGravity;
	MovementComponent->Gravity = GravityMultiplier * WorldGravity;
}

void ACyberShooterPlayer::ResetGravity()
{
	UCyberShooterGameInstance* instance = Cast<UCyberShooterGameInstance>(GetWorld()->GetGameInstance());
	if (instance != nullptr)
	{
		WorldGravity = instance->GetGravity();
		MovementComponent->Gravity = GravityMultiplier * WorldGravity;
	}
}

void ACyberShooterPlayer::SetAirFriction(float NewFriction)
{
	MovementComponent->AirFriction = NewFriction;
}

void ACyberShooterPlayer::ResetAirFriction()
{
	UCyberShooterGameInstance* instance = Cast<UCyberShooterGameInstance>(GetWorld()->GetGameInstance());
	if (instance != nullptr)
	{
		MovementComponent->AirFriction = instance->GetAirFriction();
	}
}

void ACyberShooterPlayer::SetFriction(float NewFriction)
{
	MovementComponent->Friction = NewFriction;
}

void ACyberShooterPlayer::ResetFriction()
{
	MovementComponent->Friction = 1.0f;
}

void ACyberShooterPlayer::SetMass(float Multiplier)
{
	MovementComponent->Mass = Mass * Multiplier;
}

void ACyberShooterPlayer::ResetMass()
{
	MovementComponent->Mass = Mass;
}

void ACyberShooterPlayer::SetTickRate(float NewRate)
{
	if (NewRate > 0.0f)
	{
		TickSpeed = NewRate;
		MovementComponent->SetTickSpeed(NewRate);
	}
}

float ACyberShooterPlayer::GetMass() const
{
	return MovementComponent->Mass;
}

float ACyberShooterPlayer::GetWeight() const
{
	return MovementComponent->GetGravity() * Mass;
}

/// IOrientationInterface ///

bool ACyberShooterPlayer::SetOrientation(FVector NewForward, FVector NewUp)
{
	return ApplyOrientation(NewForward, NewUp);
}

bool ACyberShooterPlayer::CheckOrientation(FVector DesiredUp)
{
	return GetUpVector().Equals(DesiredUp.GetSafeNormal());
}

/// Blueprint interface ///

void ACyberShooterPlayer::BPAddImpulse(FVector Force)
{
	AddImpulse(Force);
}

void ACyberShooterPlayer::BPAddRelativeImpulse(FVector Force)
{
	AddRelativeImpulse(Force);
}

/// Accessor functions ///

bool ACyberShooterPlayer::IsFalling() const
{
	return MovementComponent->IsFalling();
}

bool ACyberShooterPlayer::IsStable() const
{
	return MovementComponent->IsStable();
}

void ACyberShooterPlayer::SelectWeapon(int32 Slot)
{
	if (Slot >= 0 && Slot < WeaponSet.Num())
	{
		Weapon = WeaponSet[Slot];
	}
}

void ACyberShooterPlayer::AddWeapon(UWeapon* NewWeapon)
{
	// Avoid adding duplicate weapons
	for (int32 i = 0; i < WeaponSet.Num(); ++i)
	{
		if (WeaponSet[i] == NewWeapon)
		{
			return;
		}
	}

	WeaponSet.Add(NewWeapon);
}

void ACyberShooterPlayer::SelectAbility(int32 Slot)
{
	if (Slot >= 0 && Slot < AbilitySet.Num())
	{
		Ability = AbilitySet[Slot];
	}
}

void ACyberShooterPlayer::AddAbility(UAbility* NewAbility)
{
	// Avoid adding duplicate weapons
	for (int32 i = 0; i < AbilitySet.Num(); ++i)
	{
		if (AbilitySet[i] == NewAbility)
		{
			return;
		}
	}

	AbilitySet.Add(NewAbility);
}

void ACyberShooterPlayer::SetSpeedMultiplier(float NewValue)
{
	if (NewValue > 0.0f)
	{
		SpeedMultiplier = NewValue;
	}
}

void ACyberShooterPlayer::SetGravityMultiplier(float NewValue)
{
	GravityMultiplier = NewValue;
	MovementComponent->Gravity = GravityMultiplier * WorldGravity;
}

void ACyberShooterPlayer::StopGravity()
{
	MovementComponent->StopGravity();
}

void ACyberShooterPlayer::StopMovement()
{
	MovementComponent->StopMovement();
}

void ACyberShooterPlayer::Teleport(ATeleportBase* Teleporter, ATeleportBase* Target)
{
	if (Teleporter != LastTeleportTarget)
	{
		LastTeleportTarget = Target;
		CameraStartPosition = GetActorLocation();

		// Teleport to the designated teleporter
		Target->OrientPlayer(this);
		MovementComponent->Teleport(Target->GetActorLocation());

		CameraDesiredPosition = Target->GetActorLocation();
		TransitionPosition = true;
	}
}

bool ACyberShooterPlayer::CheckTeleport(ATeleportBase* Target)
{
	if (Target == LastTeleportTarget)
	{
		// Reset the teleport target
		LastTeleportTarget = nullptr;

		return false;
	}
	return true;
}

bool ACyberShooterPlayer::ApplyOrientation(FVector NewForward, FVector NewUp, bool SnapCamera)
{
	// Normalize the vectors
	NewForward = NewForward.GetSafeNormal();
	NewUp = NewUp.GetSafeNormal();

	// Make sure the vectors are orthogonal
	if (!NewForward.IsNearlyZero() && !NewUp.IsNearlyZero())
	{
		if (FVector::Orthogonal(NewForward, NewUp))
		{
			// Compute the rotation corresponding to the new orientation
			FRotator world_rotation = UKismetMathLibrary::MakeRotationFromAxes(NewForward, FVector::CrossProduct(NewUp, NewForward), NewUp);

			// Change the pawn's rotation to match the new orientation
			RootComponent->SetWorldRotation(world_rotation);

			// Set the movement component to match our orientation
			MovementComponent->SetOrientation(GetForwardVector(), GetUpVector());

			if (SnapCamera)
			{
				// Rotate the camera to match the new orientation
				SpringArmComponent->SetRelativeRotation(world_rotation);
				SpringArmComponent->AddLocalRotation(CameraAngle);
				CameraComponent->SetRelativeRotation(SpringArmComponent->GetRelativeRotation());
			}
			else
			{
				// Set up the camera transition
				CameraStartRotation = SpringArmComponent->GetRelativeRotation().Quaternion();
				SpringArmComponent->SetRelativeRotation(world_rotation);
				SpringArmComponent->AddLocalRotation(CameraAngle);
				CameraDesiredRotation = SpringArmComponent->GetRelativeRotation().Quaternion();

				// Go back to the original rotation and start the transition
				SpringArmComponent->SetRelativeRotation(CameraStartRotation);
				ACyberShooterPlayerController* controller = Cast<ACyberShooterPlayerController>(Controller);
				if (controller != nullptr)
				{
					controller->SetCameraTransition(true);
					CameraTimer = CameraTransitionTime * controller->GetTransitionSpeed();
				}
				else
				{
					CameraTimer = CameraTransitionTime;
				}
			}

			return true;
		}
	}

	return false;
}

void ACyberShooterPlayer::CameraTransition(float DeltaTime)
{
	CameraTimer -= DeltaTime;
	if (CameraTimer > 0.0f)
	{
		float dilation = 1.0f;
		ACyberShooterPlayerController* controller = Cast<ACyberShooterPlayerController>(Controller);
		if (controller != nullptr)
		{
			dilation = controller->GetTransitionSpeed();
		}

		float timer = CameraTimer / (CameraTransitionTime * dilation);

		// Interpolate the camera rotation
		SpringArmComponent->SetRelativeRotation(FQuat::Slerp(CameraDesiredRotation, CameraStartRotation, timer));
		CameraComponent->SetRelativeRotation(SpringArmComponent->GetRelativeRotation());

		// Interpolate camera position
		if (TransitionPosition)
		{
			SpringArmComponent->SetRelativeLocation(FMath::Lerp(CameraDesiredPosition, CameraStartPosition, timer));
		}
		else
		{
			SpringArmComponent->SetWorldLocation(GetActorLocation());
		}
	}
	else
	{
		// Set the final rotation
		SpringArmComponent->SetRelativeRotation(CameraDesiredRotation);
		CameraComponent->SetRelativeRotation(SpringArmComponent->GetRelativeRotation());

		// Stop the transition
		ACyberShooterPlayerController* controller = Cast<ACyberShooterPlayerController>(Controller);
		if (controller != nullptr)
		{
			controller->SetCameraTransition(false);
		}
		TransitionPosition = false;
	}
}

void ACyberShooterPlayer::SetRespawn()
{
	// Set a new respawn point
	RespawnPoints[CurrentRespawn].Location = GetActorLocation();
	RespawnPoints[CurrentRespawn].Forward = GetForwardVector();
	RespawnPoints[CurrentRespawn].Up = GetUpVector();

	// Change the respawn index
	CurrentRespawn++;
	if (CurrentRespawn == RespawnPoints.Num())
	{
		CurrentRespawn = 0;
	}
}

FRespawnPoint& ACyberShooterPlayer::GetRespawn()
{
	return RespawnPoints[CurrentRespawn];
}
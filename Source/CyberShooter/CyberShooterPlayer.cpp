// Copyright © 2020 Brian Faubion. All rights reserved.

#include "CyberShooterPlayer.h"
#include "PlayerMovementComponent.h"
#include "CyberShooterPlayerController.h"
#include "TeleportTrigger.h"
#include "Weapon.h"
#include "Ability.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"

#include "Engine/Engine.h"

const FName ACyberShooterPlayer::MoveForwardBinding("MoveForward");
const FName ACyberShooterPlayer::MoveRightBinding("MoveRight");

ACyberShooterPlayer::ACyberShooterPlayer()
{
	// Set up a fixed camera
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("PlayerSpringArmComponent"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->SetUsingAbsoluteRotation(true);
	SpringArmComponent->TargetArmLength = 1200.f;
	SpringArmComponent->bDoCollisionTest = false;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("PlayerTopDownCamera"));
	CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
	CameraComponent->SetUsingAbsoluteRotation(true);

	MovementComponent = CreateDefaultSubobject<UPlayerMovementComponent>(TEXT("PlayerMovementComponent"));
	MovementComponent->SetUpdatedComponent(RootComponent);

	// Set properties
	Momentum = 0.0f;
	MomentumBonus = 0.5f;
	MomentumReward = 0.0f;
	MomentumPenalty = -20.0f;

	Forward = FVector(1.0f, 0.0f, 0.0f);
	Up = FVector(0.0f, 0.0f, 1.0f);
	PreviousForward = Forward;
	PreviousUp = Up;
	SpeedMultiplier = 1.0f;
	GravityMultiplier = 1.0f;

	CameraAngle = FRotator(-80.0f, 0.0f, 0.0f);
	CameraTransitionTime = 1.0f;

	// Set other values
	LastTeleportTarget = nullptr;
	CameraStartRotation = FQuat();
	CameraDesiredRotation = FQuat();
	CameraTimer = 0.0f;
}

/// APawn Functions ///

void ACyberShooterPlayer::BeginPlay()
{
	Super::BeginPlay();

	// Set up the movement component
	MovementComponent->SetCollision(CollisionComponent);
	if (!SetOrientation(Forward, Up, true))
	{
		// Reset forward and up vectors if invalid vectors are set
		Forward = FVector(1.0f, 0.0f, 0.0f);
		Up = FVector(0.0f, 0.0f, 1.0f);

		MovementComponent->SetOrientation(Forward, Up);
	}
}

void ACyberShooterPlayer::Tick(float DeltaSeconds)
{
	// Get the current rotation of the pawn relative to the up and forward vectors
	FRotator world_rotation = UKismetMathLibrary::MakeRotationFromAxes(Forward, FVector::CrossProduct(Up, Forward), Up);

	// Handle camera transitions
	if (CameraTimer > 0.0f)
	{
		CameraTransition(DeltaSeconds);
	}
	
	// Drain momentum if the player is using an ability
	SustainAbility(DeltaSeconds);
	
	// Adjust movement attributes
	MovementComponent->SetSpeed((1.0f + (Momentum / MaxMomentum) * MomentumBonus) * SpeedMultiplier);
	//MovementComponent->SetGravity(GravityMultiplier);

	// Apply movement inputs
	FVector move_direction = FVector(GetInputAxisValue(MoveForwardBinding), GetInputAxisValue(MoveRightBinding), 0.0f).GetClampedToMaxSize(1.0f);
	AddMovementInput(move_direction);

	// Rotate to face the movement direction
	if (move_direction.SizeSquared() > 0.0f)
	{
		RootComponent->SetWorldRotation(world_rotation.RotateVector(move_direction).Rotation());
	}

	// Try to fire a shot if the radial menus aren't open and the player is shooting
	ACyberShooterPlayerController* controller = Cast<ACyberShooterPlayerController>(GetController());
	if (controller != nullptr && !controller->IsMenuOpen() && FireWeapon)
	{
		if (FireWeapon)
		{
			// Fire in the direction the player is aiming or in the direction the pawn is facing
			FVector fire_direction = world_rotation.RotateVector(controller->GetFireDirection());
			if (fire_direction.IsNearlyZero())
			{
				fire_direction = GetActorRotation().Vector();
			}
			FireShot(fire_direction, Up);
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

/// IBreakable Functions ///

void ACyberShooterPlayer::Kill()
{
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, "Player has been killed");
}

/// Accessor functions ///

bool ACyberShooterPlayer::IsFalling() const
{
	return MovementComponent->IsFalling();
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

bool ACyberShooterPlayer::SetOrientation(FVector NewForward, FVector NewUp, bool SnapCamera)
{
	// Normalize the vectors
	NewForward = NewForward.GetSafeNormal();
	NewUp = NewUp.GetSafeNormal();

	// Skip if we are already at the provided orientation
	if (NewForward == Forward && NewUp == Up)
		return false;

	// Make sure the vectors are orthogonal
	if (!NewForward.IsNearlyZero() && !NewUp.IsNearlyZero())
	{
		if (FVector::Orthogonal(NewForward, NewUp))
		{
			PreviousForward = Forward;
			PreviousUp = Up;

			Forward = NewForward;
			Up = NewUp;

			// Set the movement component to match our orientation
			MovementComponent->SetOrientation(Forward, Up);

			// Change the pawn's rotation to match the new orientation
			FRotator world_rotation = UKismetMathLibrary::MakeRotationFromAxes(Forward, FVector::CrossProduct(Up, Forward), Up);
			RootComponent->SetWorldRotation(world_rotation.RotateVector(FVector(1.0f, 0.0f, 0.0f)).Rotation());

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

bool ACyberShooterPlayer::RevertOrientation(bool SnapCamera)
{
	return SetOrientation(PreviousForward, PreviousUp, SnapCamera);
}

bool ACyberShooterPlayer::CheckOrientation(FVector RequiredUp) const
{
	return Up.GetSafeNormal().Equals(RequiredUp.GetSafeNormal());
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
	MovementComponent->SetGravity(GravityMultiplier * WorldGravity);
}

void ACyberShooterPlayer::SetWorldGravity(float NewValue)
{
	WorldGravity = NewValue;
	MovementComponent->SetGravity(GravityMultiplier * WorldGravity);
}

void ACyberShooterPlayer::SetGravityEnabled(bool Enable)
{
	MovementComponent->EnableGravity(Enable);
}

void ACyberShooterPlayer::AddImpulse(FVector Impulse)
{
	if (!Impulse.IsNearlyZero())
	{
		MovementComponent->AddImpulse(Impulse);
	}
}

void ACyberShooterPlayer::AddRelativeImpulse(FVector Impulse)
{
	if (!Impulse.IsNearlyZero())
	{
		// Rotate the imput vector to make it relative to the player's frame of reference
		FRotator world_rotation = UKismetMathLibrary::MakeRotationFromAxes(Forward, FVector::CrossProduct(Up, Forward), Up);
		MovementComponent->AddImpulse(world_rotation.RotateVector(Impulse));
	}
}

void ACyberShooterPlayer::StopGravity()
{
	MovementComponent->StopGravity();
}

void ACyberShooterPlayer::Teleport(ATeleportBase* Teleporter, ATeleportBase* Target)
{
	if (Teleporter != LastTeleportTarget)
	{
		LastTeleportTarget = Target;

		// Teleport to the designated teleporter
		TeleportTo(Target->GetActorLocation(), GetActorRotation());
		Target->OrientPlayer(this);
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

		// Interpolate the camera rotation
		SpringArmComponent->SetRelativeRotation(FQuat::Slerp(CameraDesiredRotation, CameraStartRotation, CameraTimer / (CameraTransitionTime * dilation)));
		CameraComponent->SetRelativeRotation(SpringArmComponent->GetRelativeRotation());
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
	}
}
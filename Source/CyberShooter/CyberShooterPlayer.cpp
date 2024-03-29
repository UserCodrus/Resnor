// Copyright © 2020 Brian Faubion. All rights reserved.

#include "CyberShooterPlayer.h"
#include "PhysicsMovementComponent.h"
#include "CyberShooterPlayerController.h"
#include "CyberShooterGameInstance.h"
#include "CyberShooterEnemy.h"
#include "CyberShooterSave.h"
#include "TeleportTrigger.h"
#include "LevelTrigger.h"
#include "Weapon.h"
#include "Ability.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialParameterCollectionInstance.h"
#include "EngineUtils.h"

#include "Engine/Engine.h"

const FName ACyberShooterPlayer::MoveForwardBinding("MoveForward");
const FName ACyberShooterPlayer::MoveRightBinding("MoveRight");

ACyberShooterPlayer::ACyberShooterPlayer()
{
	// Set up a fixed camera
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("PlayerSpringArmComponent"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->TargetArmLength = 1200.f;
	SpringArmComponent->bDoCollisionTest = false;
	SpringArmComponent->bEnableCameraLag = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("PlayerTopDownCamera"));
	CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);

	MovementComponent = CreateDefaultSubobject<UPlayerMovementComponent>(TEXT("PlayerMovementComponent"));
	MovementComponent->SetUpdatedComponent(RootComponent);

	CollisionComponent->SetCollisionProfileName("Player");
	CollisionComponent->OnComponentHit.AddDynamic(this, &ACyberShooterPlayer::OnHit);

	DummyCollisionComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("DummyCollisionComponent"));
	DummyCollisionComponent->SetCollisionProfileName("Player");
	DummyCollisionComponent->SetAbsolute(true, true, false);
	DummyCollisionComponent->SetupAttachment(RootComponent);

	// Set properties
	Momentum = 0.0f;
	MomentumBonus = 0.1f;
	MomentumReward = 0.0f;
	MomentumOverchargeDuration = 2.0f;
	Keys = 0;

	SpeedMultiplier = 1.0f;
	GravityMultiplier = 1.0f;
	Mass = 1.0f;
	MassMultiplier = 1.0f;
	Friction = 1.0f;
	FrictionMultiplier = 1.0f;
	RespawnDistance = 10000.0f;
	DamageImmunity = DAMAGETYPE_PLAYER;
	RespawnCooldown = 1.0f;
	DamageCooldownDuration = 0.5f;

	CameraAngle = FRotator(-75.0f, 0.0f, 0.0f);
	CameraTransitionTime = 1.0f;

	// Set other values
	RespawnPoints.SetNum(3);
	CurrentRespawn = 0;

	LastTeleportTarget = nullptr;
	CameraStartRotation = FQuat();
	CameraDesiredRotation = FQuat();
	CameraStartPosition = FVector(0.0f);
	CameraDistance = 1500.0f;
	DesiredDistance = 1500.0f;
	CameraZoomSpeed = 1000.0f;
	CameraLag = 20.0f;
	TransitionPosition = false;
	CameraTimer = 0.0f;
	FloorDamageTimer = 0.0f;
	GhostZones = 0;
	Warp = false;
	OcclusionRadius = 100.0f;
	OcclusionTime = 0.5f;
	OcclusionSize = 0.0f;

	Ephemeral = false;
	Moved = false;
}

/// APawn Functions ///

void ACyberShooterPlayer::BeginPlay()
{
	Super::BeginPlay();

	// Load player attributes from saved data
	UCyberShooterGameInstance* instance = Cast<UCyberShooterGameInstance>(GetWorld()->GetGameInstance());
	if (instance != nullptr)
	{
		GameInstanceData = instance;

		// Move the player to a level that matches the exit from the previous level
		if (instance->LocationID >= 0)
		{
			for (TActorIterator<ALevelTrigger> itr(GetWorld()); itr; ++itr)
			{
				// If the exit matches our current locationID, move there
				if (itr->GetID() == instance->LocationID)
				{
					SetActorLocation(itr->GetActorLocation());
					ApplyOrientation(itr->GetActorForwardVector(), itr->GetActorUpVector(), true);
					break;
				}
			}
		}

		// Load save data for the player
		const UCyberShooterSave* save = instance->GetSaveData();
		if (save != nullptr)
		{
			LoadData(save);

			if (instance->LocationID < 0)
			{
				// Move the player to a checkpoint
				SetActorLocation(save->Location);
			}
		}
	}

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

	// Set up the camera
	ResetCameraDistance();
	SpringArmComponent->TargetArmLength = DesiredDistance;
	SpringArmComponent->SetRelativeRotation(CameraAngle);
	SpringArmComponent->CameraLagSpeed = CameraLag;

	DefaultCollision = CollisionComponent->GetCollisionObjectType();

	// Disable the collision dummy
	DummyCollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DummyCollisionComponent->SetVisibility(false, true);

	// Set up material parameters
	if (CameraParameterCollection != nullptr)
	{
		CameraParameters = GetWorld()->GetParameterCollectionInstance(CameraParameterCollection);
	}
}

void ACyberShooterPlayer::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Set tick speed
	DeltaSeconds *= TickSpeed;

	// Get the current rotation of the pawn relative to the up and forward vectors
	FRotator world_rotation = GetOrientationRotator();
	
	// Adjust movement attributes
	MovementComponent->SetSpeed(GetMomentumBonus() * SpeedMultiplier);

	// Apply movement inputs
	FVector move_direction = FVector(GetInputAxisValue(MoveForwardBinding), GetInputAxisValue(MoveRightBinding), 0.0f).GetClampedToMaxSize(1.0f);
	if (!move_direction.IsZero())
	{
		MovementComponent->AddControlInput(move_direction);
		Moved = true;
	}

	// Handle camera transitions
	if (CameraTimer > 0.0f)
	{
		CameraTransition(DeltaSeconds, move_direction);
	}
	else
	{
		// Rotate to face the movement direction
		if (move_direction.SizeSquared() > 0.0f)
		{
			CoreComponent->SetRelativeRotation(move_direction.Rotation());
		}
	}

	if (SpringArmComponent->TargetArmLength < DesiredDistance)
	{
		SpringArmComponent->TargetArmLength += CameraZoomSpeed * DeltaSeconds;
		if (SpringArmComponent->TargetArmLength > DesiredDistance)
		{
			SpringArmComponent->TargetArmLength = DesiredDistance;
		}
	}
	else if (SpringArmComponent->TargetArmLength > DesiredDistance)
	{
		SpringArmComponent->TargetArmLength -= CameraZoomSpeed * DeltaSeconds;
		if (SpringArmComponent->TargetArmLength < DesiredDistance)
		{
			SpringArmComponent->TargetArmLength = DesiredDistance;
		}
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
	if (MovementComponent->GetFloorDamage() > 0 || EnvironmentDamage > 0)
	{
		int32 damage = FMath::TruncToInt(FloorDamageTimer);
		Damage(MovementComponent->GetFloorDamage() + EnvironmentDamage, DAMAGETYPE_ENVIRONMENT, EnvironmentRumble);
	}

	// Manage respawns
	if (IsFalling() && !MovementComponent->IsTeleporting())
	{
		// Respawn the player after they have fallen too far
		if (FVector::DistSquared(GetActorLocation(), GetRespawn().Location) > RespawnDistance * RespawnDistance)
		{
			Respawn();
		}
	}
	else
	{
		if (GhostZones == 0 && !Warp)
		{
			// Set the player's respawn point
			if (IsStable())
			{
				SetRespawn();
			}
		}
	}

	// Check to see if the player is occluded from the camera
	FHitResult trace;
	if (GetWorld()->LineTraceSingleByChannel(trace, CameraComponent->GetComponentLocation(), GetActorLocation(), ECollisionChannel::ECC_Visibility))
	{
		if (OcclusionSize < OcclusionRadius)
		{
			OcclusionSize += OcclusionRadius / OcclusionTime * DeltaSeconds;
			if (OcclusionSize > OcclusionRadius)
			{
				OcclusionSize = OcclusionRadius;
			}
		}
	}
	else
	{
		if (OcclusionSize > 0.0f)
		{
			OcclusionSize -= OcclusionRadius / OcclusionTime * DeltaSeconds;
			if (OcclusionSize < 0.0f)
			{
				OcclusionSize = 0.0f;
			}
		}
	}

	// Set the location of the camera mask
	if (CameraParameters != nullptr)
	{
		// Set the occlusion radius to our desired size
		CameraParameters->SetScalarParameterValue("OcclusionRadius", OcclusionSize);

		// Put the farthest edge of the capsule at the player's center
		FVector offset = GetActorLocation() - CameraComponent->GetComponentLocation();
		offset *= 1.0f - OcclusionRadius / SpringArmComponent->TargetArmLength;
		CameraParameters->SetVectorParameterValue("LookLocation", CameraComponent->GetComponentLocation() + offset);

		CameraParameters->SetVectorParameterValue("CameraLocation", CameraComponent->GetComponentLocation());
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
			if (object->CanMove())
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

		// Take damage when hitting enemies
		AEnemyBase* enemy = Cast<AEnemyBase>(OtherActor);
		if (enemy != nullptr)
		{
			enemy->ApplyImpact(this, HitComp);
		}
	}
}

/// ICombatInterface ///

bool ACyberShooterPlayer::Damage(int32 Value, int32 DamageType, UForceFeedbackEffect* RumbleEffect, UPrimitiveComponent* HitComp, AActor* Source, AActor* Origin)
{
	if (Warp)
	{
		EndWarp();

		// Only take damage during warp if the collision dummy was hit
		if (HitComp != DummyCollisionComponent)
		{
			return false;
		}
	}

	// Apply damage
	if (Super::Damage(Value, DamageType, RumbleEffect, HitComp, Source, Origin))
	{
		// Apply the rumble effect
		if (RumbleEffect != nullptr)
		{
			ACyberShooterPlayerController* controller = Cast<ACyberShooterPlayerController>(GetController());
			if (controller != nullptr)
			{
				controller->ClientPlayForceFeedback(RumbleEffect);
			}
		}

		return true;
	}

	return false;
}

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

bool ACyberShooterPlayer::CanMove()
{
	return MovementComponent->IsEnabled() && !HasIFrames();
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
	Friction = NewFriction;
	MovementComponent->Friction = Friction * FrictionMultiplier;
}

void ACyberShooterPlayer::ResetFriction()
{
	Friction = 1.0f;
	MovementComponent->Friction = Friction * FrictionMultiplier;
}

void ACyberShooterPlayer::SetMass(float NewMass)
{
	Mass = NewMass;
	MovementComponent->Mass = Mass * MassMultiplier;
}

void ACyberShooterPlayer::ResetMass()
{
	Mass = 1.0f;
	MovementComponent->Mass = Mass * MassMultiplier;
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

bool ACyberShooterPlayer::LoadData(const UCyberShooterSave* Save)
{
	// Make sure the save data is valid
	if (Save->MaxHealth <= 0 || Save->MaxMomentum <= 0)
		return false;

	// Copy data from the save game
	MaxHealth = Save->MaxHealth;
	MaxMomentum = Save->MaxMomentum;
	Keys = Save->TotalKeys;

	WeaponSet = Save->Weapons;
	AbilitySet = Save->Abilities;

	// Set orientation
	ApplyOrientation(Save->PlayerForward, Save->PlayerUp, true);

	// Reset player stats
	Health = MaxHealth;
	Momentum = MaxMomentum;
	
	if (WeaponSet.Num() > 0)
	{
		Weapon = WeaponSet[0];
	}
	if (AbilitySet.Num() > 0)
	{
		Ability = AbilitySet[0];
	}

	// Rebuild the HUD to show the new status
	ACyberShooterPlayerController* controller = Cast<ACyberShooterPlayerController>(GetController());
	if (controller != nullptr)
	{
		controller->RebuildHUD();
	}

	return true;
}

void ACyberShooterPlayer::UpgradeHealth()
{
	MaxHealth += 1;
	Health = MaxHealth;

	ACyberShooterPlayerController* controller = Cast<ACyberShooterPlayerController>(GetController());
	if (controller != nullptr)
	{
		controller->RebuildHUD();
	}
}

void ACyberShooterPlayer::UpgradeMomentum()
{
	MaxMomentum += MomentumBlockSize;
	Momentum = MaxMomentum * 2;

	ACyberShooterPlayerController* controller = Cast<ACyberShooterPlayerController>(GetController());
	if (controller != nullptr)
	{
		controller->RebuildHUD();
	}
}

void ACyberShooterPlayer::AddKey()
{
	Keys++;
}

void ACyberShooterPlayer::Refill()
{
	Health = MaxHealth;

	if (Momentum < MaxMomentum)
	{
		Momentum = MaxMomentum;
	}
}

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

int32 ACyberShooterPlayer::GetSelectedWeapon()
{
	for (int32 i = 0; i < WeaponSet.Num(); ++i)
	{
		if (WeaponSet[i] == Weapon)
		{
			return i;
		}
	}
	return 0;
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

int32 ACyberShooterPlayer::GetSelectedAbility()
{
	for (int32 i = 0; i < AbilitySet.Num(); ++i)
	{
		if (AbilitySet[i] == Ability)
		{
			return i;
		}
	}
	return 0;
}

void ACyberShooterPlayer::DisableAbility()
{
	StopAbility();
	CanUseAbility = false;
}

void ACyberShooterPlayer::EnableAbility()
{
	CanUseAbility = true;
}

void ACyberShooterPlayer::EnableGhostCollision()
{
	CollisionComponent->SetCollisionObjectType(GhostCollision);
}

void ACyberShooterPlayer::DisableGhostCollision()
{
	// Respawn if we are inside a ghost zone when ghost collision disables
	if (GhostZones != 0)
	{
		Respawn();
	}

	CollisionComponent->SetCollisionObjectType(DefaultCollision);
}

void ACyberShooterPlayer::StartWarp()
{
	if (!Warp)
	{
		// Enable the collision dummy and move it to the player
		DummyCollisionComponent->SetWorldLocation(RootComponent->GetComponentLocation());
		DummyCollisionComponent->SetWorldRotation(RootComponent->GetComponentRotation());
		DummyCollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		DummyCollisionComponent->SetVisibility(true, true);

		Warp = true;
	}
}

void ACyberShooterPlayer::EndWarp()
{
	if (Warp)
	{
		// Set up camera transition
		CameraStartPosition = GetActorLocation();
		TransitionPosition = true;

		// Move the player back to the start of the warp
		ApplyOrientation(DummyCollisionComponent->GetForwardVector(), DummyCollisionComponent->GetUpVector(), true);
		MovementComponent->Teleport(DummyCollisionComponent->GetComponentLocation());

		// Disable the collision dummy
		DummyCollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		DummyCollisionComponent->SetVisibility(false, true);

		Warp = false;
		StopAbility();
	}
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

void ACyberShooterPlayer::SetMassMultiplier(float NewValue)
{
	MassMultiplier = NewValue;
	MovementComponent->Mass = Mass * MassMultiplier;
}

void ACyberShooterPlayer::SetFrictionMultiplier(float NewValue)
{
	FrictionMultiplier = NewValue;
	MovementComponent->Friction = Friction * FrictionMultiplier;
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
	if (Teleporter != LastTeleportTarget && !Warp)
	{
		LastTeleportTarget = Target;
		CameraStartPosition = GetActorLocation();
		SpringArmComponent->CameraLagSpeed = 0.0f;

		// Teleport to the designated teleporter
		Target->OrientPlayer(this);
		MovementComponent->Teleport(Target->GetActorLocation());

		TransitionPosition = true;
		ShowPawn = false;

		// Make the player invicible during the teleport
		DamageCooldown = DamageCooldownDuration + CameraTransitionTime;

		// Spawn teleport particles
		if (TeleportStartParticles != nullptr)
		{
			FTransform transform;
			transform.SetLocation(GetActorLocation());
			transform.SetRotation(GetActorRotation().Quaternion());
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TeleportStartParticles, transform);
		}

		// Reset respawn points to the teleport location
		for (int32 i = 0; i < RespawnPoints.Num(); ++i)
		{
			RespawnPoints[i].Location = Target->GetActorLocation();
			RespawnPoints[i].Forward = GetForwardVector();
			RespawnPoints[i].Up = GetUpVector();
		}
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

			// Set up the camera transition
			if (!SnapCamera)
			{
				CameraStartRotation = SpringArmComponent->GetComponentRotation().Quaternion();
				PlayerStartRotation = CoreComponent->GetComponentRotation().Quaternion();

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

			// Change the pawn's rotation to match the new orientation
			RootComponent->SetWorldRotation(world_rotation);

			// Register the desired rotation of the camera
			SpringArmComponent->SetRelativeRotation(CameraAngle);
			CameraDesiredRotation = SpringArmComponent->GetComponentRotation().Quaternion();

			return true;
		}
	}

	return false;
}

void ACyberShooterPlayer::CameraTransition(float DeltaTime, FVector MovementDirection)
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

		float timer = FMath::SmoothStep(0.1f, 0.9f, CameraTimer / (CameraTransitionTime * dilation));

		// Interpolate the camera rotation
		SpringArmComponent->SetWorldRotation(FQuat::Slerp(CameraDesiredRotation, CameraStartRotation, timer));

		// Interpolate character rotation
		CoreComponent->SetWorldRotation(FQuat::Slerp(RootComponent->GetComponentQuat(), PlayerStartRotation, timer));

		// Interpolate camera position
		if (TransitionPosition)
		{
			SpringArmComponent->SetWorldLocation(FMath::Lerp(GetActorLocation(), CameraStartPosition, timer));
			CoreComponent->SetWorldLocation(FMath::Lerp(GetActorLocation(), CameraStartPosition, timer));
		}
	}
	else
	{
		// Set the final position
		SpringArmComponent->SetRelativeRotation(CameraAngle);
		SpringArmComponent->SetRelativeLocation(FVector::ZeroVector);
		CoreComponent->SetRelativeLocation(FVector::ZeroVector);

		// Stop the transition
		ACyberShooterPlayerController* controller = Cast<ACyberShooterPlayerController>(Controller);
		if (controller != nullptr)
		{
			controller->SetCameraTransition(false);
		}

		if (TransitionPosition)
		{
			// Spawn teleport particles
			if (TeleportEndParticles != nullptr)
			{
				FTransform transform;
				transform.SetLocation(GetActorLocation());
				transform.SetRotation(GetActorRotation().Quaternion());
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TeleportEndParticles, transform);
			}

			// Reset camera lag
			SpringArmComponent->CameraLagSpeed = CameraLag;

			TransitionPosition = false;
			ShowPawn = true;
		}
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

FVector ACyberShooterPlayer::GetRespawnLocation()
{
	return GetRespawn().Location;
}

void ACyberShooterPlayer::ForceRespawn()
{
	Respawn();
}

void ACyberShooterPlayer::Respawn()
{
	if (!Warp)
	{
		// Return to the current respawn point
		FRespawnPoint respawn = GetRespawn();
		MovementComponent->Teleport(respawn.Location);
		ApplyOrientation(respawn.Forward, respawn.Up, true);

		// Apply damage
		Damage(1, DAMAGETYPE_ENVIRONMENT, FallRumble);
		DamageCooldown = RespawnCooldown;

		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, "Player respawned");
	}
	else
	{
		// Cancel warp
		EndWarp();
	}

	// Cancel abilities
	StopAbility();
}

void ACyberShooterPlayer::EnterGhostZone()
{
	GhostZones++;
}

void ACyberShooterPlayer::ExitGhostZone()
{
	GhostZones--;
}

void ACyberShooterPlayer::SetCameraDistance(float Distance)
{
	DesiredDistance = Distance;
}

void ACyberShooterPlayer::ResetCameraDistance()
{
	DesiredDistance = CameraDistance;
}
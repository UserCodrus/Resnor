// Copyright © 2020 Brian Faubion. All rights reserved.

#include "CyberShooterEnemy.h"
#include "CyberShooterPlayer.h"
#include "EnemyTurretAI.h"
#include "EnemySeekerAI.h"
#include "SplineMovementComponent.h"
#include "PhysicsMovementComponent.h"
#include "CyberShooterGameInstance.h"
#include "Spawner.h"
#include "AggroZone.h"
#include "BulletMovementComponent.h"

#include "Components/CapsuleComponent.h"
#include "Components/SplineComponent.h"

///
/// AEnemyBase ///
///

AEnemyBase::AEnemyBase()
{
	// Create the aiming component
	AimComponent = CreateDefaultSubobject<USceneComponent>(TEXT("AimComponent"));
	AimComponent->SetupAttachment(RootComponent);

	CollisionComponent->SetCollisionProfileName("Enemy");

	// Apply the hit event handler
	CollisionComponent->OnComponentHit.AddDynamic(this, &AEnemyBase::OnHit);

	// Set defaults
	Momentum = MaxMomentum;
	DamageImmunity = DAMAGETYPE_ENEMY;

	DeathWeapon = nullptr;
	CollisionForce = 1000.0f;
	ContactDamage = 1;

	AggroLevel = 0;
	RequiredAggro = 1;
	MinimumAggro = 0;
	AIDistance = 500.0f;

	SafeRespawn = true;
	OutOfBoundsRespawn = true;
	RespawnDuration = 5.0f;
	RespawnCooldown = 2.0f;

	RespawnTimer = 0.0f;

	Ephemeral = false;
}

void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();

	// Apply default AI settings
	AEnemyAIController* controller = Cast<AEnemyAIController>(GetController());
	if (controller != nullptr)
	{
		controller->SetAimProfile(DefaultAiming);
		controller->SetMovementProfile(DefaultMovement);
	}
}

void AEnemyBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	DeltaSeconds *= TickSpeed;

	// Fire the pawn's weapon
	if (FireWeapon)
	{
		Fire(AimComponent->GetForwardVector());
	}

	// Manage the respawn timer
	if (RespawnTimer > 0.0f)
	{
		RespawnTimer -= DeltaSeconds;
		if (RespawnTimer <= 0.0f)
		{
			if (Respawned)
			{
				// Check for collisions at the enemy's position and respawn if the location is clear, otherwise restart the spawn cooldown
				FHitResult hit;
				if (SafeRespawn && GetWorld()->SweepSingleByChannel(hit, GetActorLocation(), GetActorLocation(), GetActorQuat(), CollisionComponent->GetCollisionObjectType(), CollisionComponent->GetCollisionShape()))
				{
					RespawnTimer = RespawnCooldown;
					DamageCooldown = RespawnTimer;
				}
				else
				{
					CancelRespawn();
				}
			}
			else
			{
				if (OutOfBoundsRespawn)
				{
					// Take fall damage then respawn if possible
					Damage(1, DAMAGETYPE_NONE, nullptr);
					if (!Disabled)
					{
						Respawn();
					}
				}
				else
				{
					// Kill the pawn when out of bounds
					Kill();
				}
			}
		}
	}
}

void AEnemyBase::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Apply contact damage and knockback to players
	ACyberShooterPlayer* player = Cast<ACyberShooterPlayer>(OtherActor);
	if (player != nullptr)
	{
		/*FVector direction = GetOrientationRotator().UnrotateVector(player->GetActorLocation() - GetActorLocation());
		direction.Z = 0.0f;
		direction = GetOrientationRotator().RotateVector(direction).GetSafeNormal();

		player->AddImpulse(direction * CollisionForce);
		player->Damage(ContactDamage, DAMAGETYPE_ENEMY, nullptr, OtherComp, this, this);*/
		ApplyImpact(player, OtherComp);
	}
}

void AEnemyBase::StartRespawn()
{
	RespawnTimer = RespawnDuration;
	DamageCooldown = RespawnTimer;
}

void AEnemyBase::CancelRespawn()
{
	// Clear the respawn timer and any respawn effects
	RespawnTimer = 0.0f;
	DamageCooldown = 0.0f;
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	Respawned = false;

	// Activate AI if needed
	CheckAIEnable();
}

void AEnemyBase::Respawn()
{
	// Disable collision and set the respawn timer to prevent the enemy from popping in on top of the player
	SetActorEnableCollision(false);
	Respawned = true;
	RespawnTimer = RespawnCooldown;
	DamageCooldown = RespawnTimer;

	// Deactivate AI
	AEnemyAIController* controller = Cast<AEnemyAIController>(GetController());
	if (controller != nullptr)
	{
		controller->StopAI();
	}
}

void AEnemyBase::ApplyImpact(ACyberShooterPlayer* Player, UPrimitiveComponent* Comp)
{
	if (!Player->HasIFrames())
	{
		// Apply contact force perpendicular to our up vector
		FVector direction = GetOrientationRotator().UnrotateVector(Player->GetActorLocation() - GetActorLocation());
		direction.Z = 0.0f;
		direction = GetOrientationRotator().RotateVector(direction).GetSafeNormal();
		Player->AddImpulse(direction.GetSafeNormal() * CollisionForce);

		// Apply damage
		Player->Damage(ContactDamage, DAMAGETYPE_ENEMY, nullptr, Comp, this, this);
	}
}

/// ICombatInterface ///

bool AEnemyBase::Damage(int32 Value, int32 DamageType, UForceFeedbackEffect* RumbleEffect, UPrimitiveComponent* HitComp, AActor* Source, AActor* Origin)
{
	if (IsAggro())
	{
		return Super::Damage(Value, DamageType, RumbleEffect, HitComp, Source, Origin);
	}
	return false;
}

void AEnemyBase::Kill()
{
	// Switch to the death weapon and try to fire a shot
	ShotCooldown = 0.0f;
	UWeapon* currentweapon = Weapon;
	Weapon = DeathWeapon;
	Fire(CoreComponent->GetForwardVector());
	Weapon = currentweapon;

	// Notify parent zones that the pawn has despawned
	if (!Ephemeral)
	{
		for (int32 i = 0; i < ParentZone.Num(); ++i)
		{
			ParentZone[i]->NotifyDespawn();
		}
	}

	Super::Kill();
}

/// IAggroInterface ///

void AEnemyBase::Aggro()
{
	AggroLevel++;

	CheckAIEnable();
}

void AEnemyBase::EndAggro()
{
	AggroLevel--;

	CheckAIDisable();
}

bool AEnemyBase::IsAggro()
{
	return AggroLevel > 0;
}

void AEnemyBase::EnterAggro()
{
	CancelRespawn();
}

void AEnemyBase::ExitAggro()
{
	StartRespawn();
}

void AEnemyBase::AggroReset()
{
	if (Disabled)
	{
		EnablePawn();
	}
	else
	{
		Respawn();
	}
}

void AEnemyBase::AggroDisable()
{
	DisablePawn();
}

void AEnemyBase::RegisterZone(AAggroZone* Zone)
{
	if (!Ephemeral)
	{
		ParentZone.Add(Zone);
		Zone->NotifyRegister();
	}
}

/// Accessors ///

FVector AEnemyBase::GetAimVector() const
{
	return AimComponent->GetForwardVector();
}

FRotator AEnemyBase::GetAimRotation() const
{
	return AimComponent->GetComponentRotation();
}

FRotator AEnemyBase::GetAimLocalRotation() const
{
	return AimComponent->GetRelativeRotation();
}

void AEnemyBase::SetAimRotation(FRotator Rotation)
{
	AimComponent->SetRelativeRotation(Rotation);
}

void AEnemyBase::SetAimRotation(FQuat Rotation)
{
	AimComponent->SetRelativeRotation(Rotation);
}

void AEnemyBase::SetAimWorldRotation(FRotator Rotation)
{
	AimComponent->SetWorldRotation(Rotation);
}

void AEnemyBase::SetAimWorldRotation(FQuat Rotation)
{
	AimComponent->SetWorldRotation(Rotation);
}

void AEnemyBase::DisablePawn()
{
	if (!Disabled)
	{
		// Shut off AI
		AEnemyAIController* controller = Cast<AEnemyAIController>(GetController());
		if (controller != nullptr)
		{
			controller->StopAI();
			controller->SetActorTickEnabled(false);
		}

		ACyberShooterPawn::DisablePawn();
	}
}

void AEnemyBase::EnablePawn()
{
	if (Disabled)
	{
		// Turn AI back on
		AEnemyAIController* controller = Cast<AEnemyAIController>(GetController());
		if (controller != nullptr)
		{
			controller->SetActorTickEnabled(true);
		}

		ACyberShooterPawn::EnablePawn();

		// Reset the pawn's status
		Health = MaxHealth;
		Momentum = MaxMomentum;
		Respawn();
	}
}

/// Utilities ///

void AEnemyBase::CheckAIEnable()
{
	if (Disabled)
		return;

	if (AggroLevel >= RequiredAggro)
	{
		// Activate AI
		AEnemyAIController* controller = Cast<AEnemyAIController>(GetController());
		if (controller != nullptr && !Respawned)
		{
			controller->StartAI();
			OnAggro();
		}
	}
}

void AEnemyBase::CheckAIDisable()
{
	if (Disabled)
		return;

	if (AggroLevel <= MinimumAggro)
	{
		// Deactivate AI
		AEnemyAIController* controller = Cast<AEnemyAIController>(GetController());
		if (controller != nullptr)
		{
			controller->StopAI();
			OnEndAggro();
		}
	}
}

///
/// AEnemyTurret ///
///

AEnemyTurret::AEnemyTurret()
{
	AIControllerClass = AEnemyTurretAI::StaticClass();
}

void AEnemyTurret::BeginPlay()
{
	Super::BeginPlay();
}

///
/// AEnemyMovingTurret ///
///

AEnemyMovingTurret::AEnemyMovingTurret()
{
	Spline = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
	Spline->SetupAttachment(RootComponent);
	Spline->SetAbsolute(true, false, true);
	Spline->Duration = 10.0f;

	MovementComponent = CreateDefaultSubobject<USplineMovementComponent>(TEXT("SplineMovementComponent"));
	MovementComponent->Spline = Spline;
}

FVector AEnemyMovingTurret::GetVelocity() const
{
	return MovementComponent->Velocity;
}

/// Spline Utilities ///

void AEnemyMovingTurret::MoveEnemyToStart()
{
	SetActorLocation(Spline->GetComponentLocation());
}

void AEnemyMovingTurret::MoveEnemyToEnd()
{
	SetActorLocation(Spline->GetWorldLocationAtSplinePoint(Spline->GetNumberOfSplinePoints() - 1));
}

void AEnemyMovingTurret::ResetSpline()
{
	// Remove extra points
	for (int32 i = Spline->GetNumberOfSplinePoints() - 1; i > 0; --i)
	{
		Spline->RemoveSplinePoint(i);
	}

	// Reset the location of the spline
	Spline->SetWorldLocation(GetActorLocation());
}

void AEnemyMovingTurret::MoveSpline()
{
	Spline->SetWorldLocation(GetActorLocation());
}

void AEnemyMovingTurret::AppendSpline()
{
	Spline->AddSplineWorldPoint(GetActorLocation());
}

///
/// AEnemySeeker ///
///

AEnemySeeker::AEnemySeeker()
{
	AIControllerClass = AEnemySeekerAI::StaticClass();

	MovementComponent = CreateDefaultSubobject<UEnemyMovementComponent>(TEXT("EnemyMovementComponent"));
	MovementComponent->SetUpdatedComponent(RootComponent);

	// Set defaults
	GravityMultiplier = 1.0f;
	WorldGravity = 0.0f;
	Mass = 1.0f;
}

void AEnemySeeker::BeginPlay()
{
	Super::BeginPlay();

	// Set up the movement component
	MovementComponent->Height = CollisionComponent->GetScaledCapsuleHalfHeight();
	MovementComponent->Disable();

	// Set world physics
	ResetGravity();
	ResetAirFriction();
	ResetMass();

	RespawnLocation = GetActorLocation();
}

FVector AEnemySeeker::GetVelocity() const
{
	return MovementComponent->GetTotalVelocity();
}

void AEnemySeeker::CancelRespawn()
{
	Super::CancelRespawn();
	MovementComponent->Enable();
}

void AEnemySeeker::Respawn()
{
	Super::Respawn();
	MovementComponent->Teleport(RespawnLocation);
	MovementComponent->Disable();
}

void AEnemySeeker::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor != nullptr && OtherActor != this)
	{
		IPhysicsInterface* object = Cast<IPhysicsInterface>(OtherActor);
		if (object != nullptr)
		{
			// Handle the collision
			MovementComponent->Impact(object, Hit.Normal);
		}

		// Apply contact damage and knockback to players
		ACyberShooterPlayer* player = Cast<ACyberShooterPlayer>(OtherActor);
		if (player != nullptr)
		{
			/*if (!player->HasIFrames())
			{
				FVector direction = player->GetActorLocation() - GetActorLocation();
				player->AddImpulse(direction.GetSafeNormal() * CollisionForce);
			
				player->Damage(ContactDamage, DAMAGETYPE_ENEMY, nullptr, OtherComp, this, this);
			}*/
			ApplyImpact(player, OtherComp);
		}
	}
}

void AEnemySeeker::AddControlInput(FVector Direction)
{
	MovementComponent->AddControlInput(Direction);
}

void AEnemySeeker::DisablePawn()
{
	MovementComponent->Disable();
	AEnemyBase::DisablePawn();
}

void AEnemySeeker::EnablePawn()
{
	MovementComponent->Enable();
	AEnemyBase::EnablePawn();
}

/// IPhysicsInterface ///

FVector AEnemySeeker::GetVelocity()
{
	return MovementComponent->GetTotalVelocity();
}

void AEnemySeeker::ChangeVelocity(FVector NewVelocity)
{
	MovementComponent->SetVelocity(NewVelocity);
}

bool AEnemySeeker::CanMove()
{
	return IsAggro() && MovementComponent->IsEnabled();
}

void AEnemySeeker::AddImpulse(FVector Force)
{
	MovementComponent->AddImpulse(Force);
}

void AEnemySeeker::AddRelativeImpulse(FVector Force)
{
	MovementComponent->AddImpulse(CoreComponent->GetComponentRotation().RotateVector(Force));
}

void AEnemySeeker::AddStaticForce(FVector Force)
{
	MovementComponent->SetStaticForce(Force);
}

void AEnemySeeker::RemoveStaticForce(FVector Force)
{
	MovementComponent->SetStaticForce(-Force);
}

void AEnemySeeker::ResetStaticForce()
{
	MovementComponent->ResetStaticForce();
}

void AEnemySeeker::SetGravity(float NewGravity)
{
	WorldGravity = NewGravity;
	MovementComponent->Gravity = GravityMultiplier * WorldGravity;
}

void AEnemySeeker::ResetGravity()
{
	UCyberShooterGameInstance* instance = Cast<UCyberShooterGameInstance>(GetWorld()->GetGameInstance());
	if (instance != nullptr)
	{
		WorldGravity = instance->GetGravity();
		MovementComponent->Gravity = GravityMultiplier * WorldGravity;
	}
}

void AEnemySeeker::SetAirFriction(float NewFriction)
{
	MovementComponent->AirFriction = NewFriction;
}

void AEnemySeeker::ResetAirFriction()
{
	UCyberShooterGameInstance* instance = Cast<UCyberShooterGameInstance>(GetWorld()->GetGameInstance());
	if (instance != nullptr)
	{
		MovementComponent->AirFriction = instance->GetAirFriction();
	}
}

void AEnemySeeker::SetFriction(float NewFriction)
{
	MovementComponent->Friction = NewFriction;
}

void AEnemySeeker::ResetFriction()
{
	MovementComponent->Friction = 1.0f;
}

void AEnemySeeker::SetMass(float Multiplier)
{
	MovementComponent->Mass = Mass * Multiplier;
}

void AEnemySeeker::ResetMass()
{
	MovementComponent->Mass = Mass;
}

void AEnemySeeker::SetTickRate(float NewRate)
{
	if (NewRate > 0.0f)
	{
		TickSpeed = NewRate;
		MovementComponent->SetTickSpeed(NewRate);
	}
}

float AEnemySeeker::GetMass() const
{
	return MovementComponent->Mass;
}

float AEnemySeeker::GetWeight() const
{
	return MovementComponent->GetGravity() * Mass;
}

/// IAggroInterface ///

void AEnemySeeker::Aggro()
{
	Super::Aggro();
	MovementComponent->Enable();
}

void AEnemySeeker::EndAggro()
{
	Super::EndAggro();
	MovementComponent->Disable();
}

/// Blueprint interface ///

void AEnemySeeker::BPAddImpulse(FVector Force)
{
	AddImpulse(Force);
}

void AEnemySeeker::BPAddRelativeImpulse(FVector Force)
{
	AddRelativeImpulse(Force);
}

bool AEnemySeeker::BPIsFalling()
{
	return MovementComponent->IsFalling();
}

///
/// AEnemyMinion ///
///

AEnemyMinion::AEnemyMinion()
{
	// Delete the minion when it is killed
	Ephemeral = true;
}

void AEnemyMinion::BeginPlay()
{
	Super::BeginPlay();

	Respawn();
}

void AEnemyMinion::ClaimPawn(ASpawner* NewParent)
{
	if (Parent == nullptr)
	{
		Parent = NewParent;
		AggroLevel = Parent->GetAggro();
	}
}

bool AEnemyMinion::IsParent(AActor* Actor)
{
	return Parent == Actor;
}

void AEnemyMinion::SetAggro(int32 Required, int32 Minimum)
{
	RequiredAggro = Required;
	MinimumAggro = Minimum;
}

/// ICombatInterface ///

void AEnemyMinion::Kill()
{
	if (Parent != nullptr)
	{
		Parent->NotifyDeath(this);
	}

	Super::Kill();
}

void AEnemyMinion::QuietKill()
{
	DeathSound = nullptr;
	Kill();
}

///
/// AEnemyBouncer ///
///

AEnemyBouncer::AEnemyBouncer()
{
	// Create the movement component
	MovementComponent = CreateDefaultSubobject<UBulletMovementComponent>(TEXT("EnemyMovementComponent"));
	MovementComponent->SetUpdatedComponent(RootComponent);
}

void AEnemyBouncer::BeginPlay()
{
	Super::BeginPlay();

	// Constrain movement to prevent the spike from flying into space
	MovementComponent->SetPlaneConstraintOrigin(GetActorLocation());
	MovementComponent->SetPlaneConstraintNormal(RootComponent->GetUpVector());
	MovementComponent->SetPlaneConstraintEnabled(true);
}
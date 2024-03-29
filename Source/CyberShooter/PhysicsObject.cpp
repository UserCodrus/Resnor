// Copyright © 2020 Brian Faubion. All rights reserved.

#include "PhysicsObject.h"
#include "PhysicsMovementComponent.h"
#include "CyberShooterPlayer.h"
#include "CyberShooterGameInstance.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

///
/// APhysicsObject ///
///

APhysicsObject::APhysicsObject()
{
	// Create the movement component
	MovementComponent = CreateDefaultSubobject<UPhysicsMovementComponent>(TEXT("PhysicsMovementComponent"));
	MovementComponent->SetUpdatedComponent(RootComponent);

	// Make the mesh movable
	GetStaticMeshComponent()->Mobility = EComponentMobility::Movable;
	GetStaticMeshComponent()->SetGenerateOverlapEvents(true);
	GetStaticMeshComponent()->OnComponentHit.AddDynamic(this, &APhysicsObject::OnHit);

	// Set up custom depth rendering
	GetStaticMeshComponent()->bRenderCustomDepth = false;
	GetStaticMeshComponent()->CustomDepthStencilValue = 1;

	CanChangeOrientation = false;
	Mass = 1.0f;

	Respawned = false;
	RespawnDuration = 2.0f;
	RespawnCooldown = 2.0f;
	BlinkRate = 20;
	RespawnTimer = 0;
	StartLocked = false;

	PrimaryActorTick.bCanEverTick = true;
}

void APhysicsObject::BeginPlay()
{
	Super::BeginPlay();

	// Figure out how far to do floor checks
	FVector min, max;
	GetStaticMeshComponent()->GetLocalBounds(min, max);
	MovementComponent->Height = FMath::Abs(min.Z) * GetActorScale().Z;

	// Apply the current orientation to the movement component
	InitialForward = GetActorForwardVector();
	InitialUp = GetActorUpVector();

	RespawnPoint = GetActorLocation();

	// Set world physics
	ResetGravity();
	ResetAirFriction();
	ResetMass();

	// Lock the object
	if (StartLocked)
	{
		Disable();
	}
}

void APhysicsObject::Tick(float DeltaSeconds)
{
	// Manage respawning
	if (RespawnTimer > 0.0f)
	{
		RespawnTimer -= DeltaSeconds;

		// Blink
		int state = (int)(RespawnTimer * BlinkRate) & 2;
		SetActorHiddenInGame((bool)state);

		// End the respawn sequence
		if (RespawnTimer <= 0.0f)
		{
			if (Respawned)
			{
				// Check for collisions inside the object to ensure it doesn't respawn inside another object
				FHitResult hit;
				if (GetWorld()->SweepSingleByChannel(hit, GetActorLocation(), GetActorLocation(), GetActorQuat(), GetStaticMeshComponent()->GetCollisionObjectType(), GetStaticMeshComponent()->GetCollisionShape()))
				{
					RespawnTimer = RespawnCooldown;
				}
				else
				{
					CancelRespawn();
				}
			}
			else
			{
				Respawn();
			}
		}
	}
}

FVector APhysicsObject::GetVelocity() const
{
	return MovementComponent->GetTotalVelocity();
}

void APhysicsObject::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor != nullptr && OtherActor != this)
	{
		// Handle collisions
		IPhysicsInterface* object = Cast<IPhysicsInterface>(OtherActor);
		if (object != nullptr)
		{
			MovementComponent->Impact(object, Hit.Normal);
		}
	}
}

/// IPhysicsInterface ///

FVector APhysicsObject::GetVelocity()
{
	return MovementComponent->GetTotalVelocity();
}

void APhysicsObject::ChangeVelocity(FVector NewVelocity)
{
	MovementComponent->SetVelocity(NewVelocity);
}

bool APhysicsObject::CanMove()
{
	return MovementComponent->IsEnabled();
}

void APhysicsObject::AddImpulse(FVector Force)
{
	MovementComponent->AddImpulse(Force);
}

void APhysicsObject::AddRelativeImpulse(FVector Force)
{
	MovementComponent->AddImpulse(RootComponent->GetComponentRotation().RotateVector(Force));
}

void APhysicsObject::AddStaticForce(FVector Force)
{
	MovementComponent->SetStaticForce(Force);
}

void APhysicsObject::RemoveStaticForce(FVector Force)
{
	MovementComponent->SetStaticForce(-Force);
}

void APhysicsObject::ResetStaticForce()
{
	MovementComponent->ResetStaticForce();
}

void APhysicsObject::SetGravity(float NewGravity)
{
	MovementComponent->Gravity = NewGravity;
}

void APhysicsObject::ResetGravity()
{
	UCyberShooterGameInstance* instance = Cast<UCyberShooterGameInstance>(GetWorld()->GetGameInstance());
	if (instance != nullptr)
	{
		MovementComponent->Gravity = instance->GetGravity();
	}
}

void APhysicsObject::SetAirFriction(float NewFriction)
{
	MovementComponent->AirFriction = NewFriction;
}

void APhysicsObject::ResetAirFriction()
{
	UCyberShooterGameInstance* instance = Cast<UCyberShooterGameInstance>(GetWorld()->GetGameInstance());
	if (instance != nullptr)
	{
		MovementComponent->AirFriction = instance->GetAirFriction();
	}
}

void APhysicsObject::SetFriction(float NewFriction)
{
	MovementComponent->Friction = NewFriction;
}

void APhysicsObject::ResetFriction()
{
	MovementComponent->Friction = 1.0f;
}

void APhysicsObject::SetMass(float Multiplier)
{
	MovementComponent->Mass = Mass * Multiplier;
}

void APhysicsObject::ResetMass()
{
	MovementComponent->Mass = Mass;
}

void APhysicsObject::SetTickRate(float NewRate)
{
	if (NewRate > 0.0f)
	{
		MovementComponent->SetTickSpeed(NewRate);
	}
}

float APhysicsObject::GetMass() const
{
	return MovementComponent->Mass;
}

float APhysicsObject::GetWeight() const
{
	return MovementComponent->GetGravity() * MovementComponent->Mass;
}

/// IOrientationInterface ///

bool APhysicsObject::SetOrientation(FVector NewForward, FVector NewUp)
{
	if (CanChangeOrientation)
	{
		return SetOrientation_Internal(NewForward, NewUp);
	}

	return false;
}

bool APhysicsObject::CheckOrientation(FVector DesiredUp)
{
	return GetActorUpVector().Equals(DesiredUp.GetSafeNormal());
}

/// IAggroInterface ///

void APhysicsObject::Aggro()
{
	// Do nothing
}

void APhysicsObject::EndAggro()
{
	// Do nothing
}

bool APhysicsObject::IsAggro()
{
	return false;
}

void APhysicsObject::AggroReset()
{
	Respawn();
}

void APhysicsObject::EnterAggro()
{
	CancelRespawn();
}

void APhysicsObject::ExitAggro()
{
	StartRespawn();
}

/// Physics Object Functions ///

void APhysicsObject::Enable()
{
	MovementComponent->Enable();

	EnableObject();
}

void APhysicsObject::Disable()
{
	MovementComponent->Disable();

	DisableObject();
}

void APhysicsObject::StartRespawn()
{
	RespawnTimer = RespawnDuration;
}

void APhysicsObject::CancelRespawn()
{
	RespawnTimer = 0.0f;
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	Respawned = false;

	MovementComponent->Wake();
}

void APhysicsObject::Respawn()
{
	SetActorEnableCollision(false);
	RespawnTimer = RespawnCooldown;
	Respawned = true;

	MovementComponent->Teleport(RespawnPoint);
	SetOrientation_Internal(InitialForward, InitialUp);
}

bool APhysicsObject::SetOrientation_Internal(FVector NewForward, FVector NewUp)
{
	if (!NewForward.IsNearlyZero() && !NewUp.IsNearlyZero())
	{
		if (FVector::Orthogonal(NewForward, NewUp))
		{
			FRotator world_rotation = UKismetMathLibrary::MakeRotationFromAxes(NewForward, FVector::CrossProduct(NewUp, NewForward), NewUp);
			RootComponent->SetWorldRotation(world_rotation);
			MovementComponent->Wake();

			return true;
		}
	}

	return false;
}

///
/// AMimicObject ///
///

AMimicObject::AMimicObject()
{
	AggroLevel = 0;
	CanChangeOrientation = false;
}

void AMimicObject::BeginPlay()
{
	Super::BeginPlay();

	// Get a reference the the player
	Player = Cast<ACyberShooterPlayer>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
}

void AMimicObject::Tick(float DeltaSeconds)
{
	if (AggroLevel > 0 && Player != nullptr)
	{
		// Change orientation to match the player
		if (!CheckOrientation(Player->GetUpVector()))
		{
			FRotator world_rotation = UKismetMathLibrary::MakeRotationFromAxes(Player->GetForwardVector(), FVector::CrossProduct(Player->GetUpVector(), Player->GetForwardVector()), Player->GetUpVector());
			RootComponent->SetWorldRotation(world_rotation);
			MovementComponent->Wake();
		}
	}

	Super::Tick(DeltaSeconds);
}

/// IOrientationInterface ///

bool AMimicObject::SetOrientation(FVector NewForward, FVector NewUp)
{
	// Ignore orientation triggers
	return false;
}

/// IAggroInterface ///

void AMimicObject::Aggro()
{
	AggroLevel++;
}

void AMimicObject::EndAggro()
{
	AggroLevel--;
}

bool AMimicObject::IsAggro()
{
	return AggroLevel > 0;
}
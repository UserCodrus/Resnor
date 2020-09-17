// Copyright © 2020 Brian Faubion. All rights reserved.

#include "PhysicsZone.h"
#include "CyberShooterPlayer.h"
#include "CyberShooterProjectile.h"

#if WITH_EDITOR
#include "Components/ArrowComponent.h"
#endif

APhysicsZone::APhysicsZone()
{
	PrimaryActorTick.bCanEverTick = false;

	OnActorBeginOverlap.AddDynamic(this, &APhysicsZone::BeginOverlap);
	OnActorEndOverlap.AddDynamic(this, &APhysicsZone::EndOverlap);

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	OrientationComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Orientation"));
	OrientationComponent->SetupAttachment(RootComponent);

#if WITH_EDITOR
	OrientationUpArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("UpArrow"));
	OrientationUpArrow->SetupAttachment(OrientationComponent);
	OrientationUpArrow->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));
	OrientationUpArrow->ArrowColor = FColor::Blue;
	OrientationUpArrow->bIsEditorOnly = true;

	OrientationForwardArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("ForwardArrow"));
	OrientationForwardArrow->SetupAttachment(OrientationComponent);
	OrientationForwardArrow->ArrowColor = FColor::Red;
	OrientationForwardArrow->bIsEditorOnly = true;
#endif

	AllowGravity = true;
	Gravity = 1000.0f;

	SetOrientation = false;

	Friction = 1.0f;
	AirFriction = 0.5f;
	Force = FVector(0.0f);
	TickSpeed = 1.0f;
}

void APhysicsZone::BeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	// Set player attributes
	ACyberShooterPlayer* player = Cast<ACyberShooterPlayer>(OtherActor);
	if (player != nullptr)
	{
		if (SetOrientation)
		{
			player->SetOrientation(OrientationComponent->GetForwardVector(), OrientationComponent->GetUpVector());
		}

		player->SetWorldGravity(Gravity);
		player->SetGravityEnabled(AllowGravity);

		player->SetFriction(Friction);
		player->SetAirFriction(AirFriction);
		player->SetStaticForce(Force);
		player->SetTickSpeed(TickSpeed);
	}
	else
	{
		ACyberShooterProjectile* projectile = Cast<ACyberShooterProjectile>(OtherActor);
		if (projectile != nullptr)
		{
			projectile->SetAirFriction(AirFriction);
			projectile->SetStaticForce(Force);
			projectile->SetTickSpeed(TickSpeed);
		}
	}
}

void APhysicsZone::EndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	// Reset player attributes
	ACyberShooterPlayer* player = Cast<ACyberShooterPlayer>(OtherActor);
	if (player != nullptr)
	{
		if (SetOrientation)
		{
			player->RevertOrientation();
		}

		player->ResetWorldGravity();
		player->SetGravityEnabled(true);

		player->ResetFriction();
		player->ResetAirFriction();
		player->ResetStaticForce();
		player->SetTickSpeed(1.0f);
	}
	else
	{
		ACyberShooterProjectile* projectile = Cast<ACyberShooterProjectile>(OtherActor);
		if (projectile != nullptr)
		{
			projectile->ResetAirFriction();
			projectile->ResetStaticForce();
			projectile->ResetTickSpeed();
		}
	}
}
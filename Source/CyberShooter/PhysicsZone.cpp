// Copyright © 2020 Brian Faubion. All rights reserved.

#include "PhysicsZone.h"
#include "CyberShooterProjectile.h"
#include "PhysicsInterface.h"

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
	ACyberShooterProjectile* projectile = Cast<ACyberShooterProjectile>(OtherActor);
	if (projectile != nullptr)
	{
		projectile->SetAirFriction(AirFriction);
		projectile->SetStaticForce(Force);
		projectile->SetTickSpeed(TickSpeed);
	}
	else
	{
		// Set object attributes
		IPhysicsInterface* object = Cast<IPhysicsInterface>(OtherActor);
		if (object != nullptr)
		{
			/*if (SetOrientation)
			{
				player->SetOrientation(OrientationComponent->GetForwardVector(), OrientationComponent->GetUpVector());
			}*/

			object->SetGravity(Gravity);
			object->SetGravityEnabled(AllowGravity);

			object->SetFriction(Friction);
			object->SetAirFriction(AirFriction);
			object->AddStaticForce(Force);
			object->SetTickRate(TickSpeed);
		}
	}
}

void APhysicsZone::EndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	// Reset object attributes
	IPhysicsInterface* object = Cast<IPhysicsInterface>(OtherActor);
	if (object != nullptr)
	{
		/*if (SetOrientation)
		{
			player->RevertOrientation();
		}*/

		object->ResetGravity();
		object->SetGravityEnabled(true);

		object->ResetFriction();
		object->ResetAirFriction();
		object->ResetStaticForce();
		object->SetTickRate(1.0f);
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
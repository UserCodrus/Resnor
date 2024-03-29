// Copyright © 2020 Brian Faubion. All rights reserved.

#include "Singularity.h"
#include "CyberShooterProjectile.h"
#include "PhysicsInterface.h"

#include "Components/SphereComponent.h"

#include "Engine/Engine.h"

ASingularity::ASingularity()
{
	PrimaryActorTick.bCanEverTick = true;

	OnActorBeginOverlap.AddDynamic(this, &ASingularity::BeginOverlap);
	OnActorEndOverlap.AddDynamic(this, &ASingularity::EndOverlap);

	// Create the collision sphere
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	CollisionComponent->SetCollisionProfileName("OverlapAll");
	CollisionComponent->SetSphereRadius(50.0f);
	CollisionComponent->SetMobility(EComponentMobility::Static);
	RootComponent = CollisionComponent;
}

void ASingularity::BeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	// Add objects to the list
	IPhysicsInterface* object = Cast<IPhysicsInterface>(OtherActor);
	if (object != nullptr)
	{
		Objects.Add(OtherActor);
	}
	else
	{
		ACyberShooterProjectile* projectile = Cast<ACyberShooterProjectile>(OtherActor);
		if (projectile != nullptr)
		{
			Projectiles.Add(projectile);
		}
	}
}

void ASingularity::EndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	// Remove objects from the list
	IPhysicsInterface* object = Cast<IPhysicsInterface>(OtherActor);
	if (object != nullptr)
	{
		Objects.Remove(OtherActor);
		object->ResetStaticForce();
	}
	else
	{
		ACyberShooterProjectile* projectile = Cast<ACyberShooterProjectile>(OtherActor);
		if (projectile != nullptr)
		{
			Projectiles.Remove(projectile);
			projectile->ResetStaticForce();
		}
	}
}

void ASingularity::Tick(float DeltaTime)
{
	for (int32 i = 0; i < Objects.Num(); ++i)
	{
		// Apply force to objects
		FVector direction = GetActorLocation() - Objects[i]->GetActorLocation();
		Cast<IPhysicsInterface>(Objects[i])->AddStaticForce(direction.GetSafeNormal() * Force);
	}
	for (int32 i = 0; i < Projectiles.Num(); ++i)
	{
		// Apply force to projectiles
		FVector direction = GetActorLocation() - Projectiles[i]->GetActorLocation();
		Projectiles[i]->SetStaticForce(direction.GetSafeNormal() * Force);
	}
}
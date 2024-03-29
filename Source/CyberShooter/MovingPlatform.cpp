// Copyright © 2020 Brian Faubion. All rights reserved.

#include "MovingPlatform.h"
#include "SplineMovementComponent.h"
#include "PhysicsInterface.h"

#include "Components/SplineComponent.h"
#include "Components/StaticMeshComponent.h"

AMovingPlatform::AMovingPlatform()
{
	// Adjust mesh settings for physics
	UStaticMeshComponent* mesh = GetStaticMeshComponent();
	mesh->SetMobility(EComponentMobility::Movable);
	mesh->OnComponentHit.AddDynamic(this, &AMovingPlatform::OnHit);

	// Create the spline and movement components
	Spline = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
	Spline->SetupAttachment(RootComponent);
	Spline->SetAbsolute(true, false, true);
	Spline->Duration = 10.0f;

	MovementComponent = CreateDefaultSubobject<USplineMovementComponent>(TEXT("SplineMovementComponent"));
	MovementComponent->Spline = Spline;

	Initialized = false;
}

void AMovingPlatform::BeginPlay()
{
	Super::BeginPlay();

	Initialized = true;
}

FVector AMovingPlatform::GetVelocity() const
{
	return MovementComponent->Velocity;
}

void AMovingPlatform::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (GetVelocity().IsNearlyZero())
		return;

	if (OtherActor != nullptr && OtherActor != this)
	{
		// Apply physics impulses
		IPhysicsInterface* object = Cast<IPhysicsInterface>(OtherActor);
		if (object != nullptr)
		{
			float alignment = FMath::Clamp(-FVector::DotProduct(GetVelocity().GetSafeNormal(), Hit.ImpactNormal), 0.0f, 1.0f);
			object->AddImpulse(GetVelocity() * alignment);
		}
	}
}

bool AMovingPlatform::IsStable() const
{
	return false;
}

/// Moving Platform Functions ///

void AMovingPlatform::Activate()
{
	if (Initialized)
	{
		// Activate the platform
		MovementComponent->CanMove = true;
		MovementComponent->StartMovement();
	}
	else
	{
		MovementComponent->Reverse = true;
	}
}

void AMovingPlatform::Deactivate()
{
	if (Initialized)
	{
		// Deactivate the platform
		MovementComponent->CanMove = false;
	}
	else
	{
		MovementComponent->Reverse = false;
	}
}

void AMovingPlatform::SendToStart()
{
	if (Initialized)
	{
		MovementComponent->MoveToStart();
	}
}

void AMovingPlatform::SendToEnd()
{
	if (Initialized)
	{
		MovementComponent->MoveToEnd();
	}
}

bool AMovingPlatform::ActivatePlatform()
{
	// Start the platform
	return MovementComponent->StartMovement();
}

/// Editor Utility Functions ///

void AMovingPlatform::MovePlatformToStart()
{
	SetActorLocation(Spline->GetComponentLocation());
}

void AMovingPlatform::MovePlatformToEnd()
{
	SetActorLocation(Spline->GetWorldLocationAtSplinePoint(Spline->GetNumberOfSplinePoints() - 1));
}

void AMovingPlatform::ResetSpline()
{
	// Remove extra points
	for (int32 i = Spline->GetNumberOfSplinePoints() - 1; i > 0; --i)
	{
		Spline->RemoveSplinePoint(i);
	}

	// Reset the location of the spline
	Spline->SetWorldLocation(GetActorLocation());
}

void AMovingPlatform::MoveSpline()
{
	Spline->SetWorldLocation(GetActorLocation());
}

void AMovingPlatform::AppendSpline()
{
	Spline->AddSplineWorldPoint(GetActorLocation());
}
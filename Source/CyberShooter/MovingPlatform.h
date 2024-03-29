// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "PhysicalStaticMesh.h"

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "MovingPlatform.generated.h"

// A moving platform that changes direction after hitting a platform trigger
UCLASS(BlueprintType, Blueprintable)
class CYBERSHOOTER_API AMovingPlatform : public APhysicalStaticMesh
{
	GENERATED_BODY()
	
public:
	AMovingPlatform();

	virtual void BeginPlay() override;
	virtual FVector GetVelocity() const override;

	// Apply physics impulses on hitting obstacles
	UFUNCTION()
		virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	virtual bool IsStable() const override;

	/// Moving Platform Functions ///

	// Activate the platform and allow it to move
	UFUNCTION(BlueprintCallable)
		void Activate();
	// Deactivate the platform to prevent it from moving
	UFUNCTION(BlueprintCallable)
		void Deactivate();

	// Move the platform to its start point
	UFUNCTION(BlueprintCallable)
		void SendToStart();
	// Move the platform to its end point
	UFUNCTION(BlueprintCallable)
		void SendToEnd();

	// Called to start the platform moving when it is waiting at a destination
	UFUNCTION(BlueprintCallable)
		bool ActivatePlatform();

	/// Editor Utility Functions ///

	// Move the platform to the start point of the spline
	UFUNCTION(Category = "SplineUtilities", CallInEditor)
		void MovePlatformToStart();
	// Move the platform to the end point of the spline
	UFUNCTION(Category = "SplineUtilities", CallInEditor)
		void MovePlatformToEnd();
	// Erase all the spline's points and place it at the center of the platform
	UFUNCTION(Category = "SplineUtilities", CallInEditor)
		void ResetSpline();
	// Move the spline's start to the center of the platform
	UFUNCTION(Category = "SplineUtilities", CallInEditor)
		void MoveSpline();
	// Add a new spline point at the platform's current position
	UFUNCTION(Category = "SplineUtilities", CallInEditor)
		void AppendSpline();

protected:
	// The spline component that dictates the turret's patrol path
	UPROPERTY(Category = "Components", EditAnywhere)
		class USplineComponent* Spline;
	// The movement component for the turret
	UPROPERTY(Category = "Components", EditAnywhere)
		class USplineMovementComponent* MovementComponent;

	bool Initialized;
};

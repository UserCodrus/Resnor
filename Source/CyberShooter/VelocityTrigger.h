// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VelocityTrigger.generated.h"

#if WITH_EDITORONLY_DATA
class UArrowComponent;
#endif

// A gadget that applies a burst of force to physics objects when activated
UCLASS(BlueprintType, Blueprintable)
class CYBERSHOOTER_API AVelocityTrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	AVelocityTrigger();

	virtual void BeginPlay() override;

	// Apply force to all selected targets
	UFUNCTION(BlueprintCallable)
		void Trigger();
	// Activate the trigger
	UFUNCTION(BlueprintCallable)
		void Activate();
	// Stop the trigger from working
	UFUNCTION(BlueprintCallable)
		void Deactivate();

	/// Blueprint Events ///

	// Called when the trigger is activated
	UFUNCTION(BlueprintImplementableEvent)
		void ActivateTrigger();
	// Called when the trigger is deactivated
	UFUNCTION(BlueprintImplementableEvent)
		void DeactivateTrigger();

protected:
	/// Velocity Trigger Functions ///

	// Apply a velocity impulse to actors when they hit the trigger
	UFUNCTION()
		virtual void BeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

	// Apply force to an actor
	void ApplyForce(AActor* Target);

	/// Properties ///

	// If set to false, the trigger will not activate when an actor hits it
	UPROPERTY(Category = "VelocityTrigger", EditAnywhere)
		bool Active;
	// The strength of the velocity impulse
	UPROPERTY(Category = "VelocityTrigger", EditAnywhere)
		float Magnitude;
	// If set to true the player will require the provided up vector to use this trigger
	UPROPERTY(Category = "VelocityTrigger", EditAnywhere)
		bool RestrictOrientation;

	// The targets that will manually activate when unlocked
	UPROPERTY(Category = "VelocityTrigger", EditInstanceOnly, BlueprintReadOnly, meta = (MustImplement = "PhysicsInterface"))
		TArray<AActor*> Targets;

	// The component that controls the direction of the velocity
	UPROPERTY(Category = "Components", EditAnywhere)
		USceneComponent* DirectionComponent;

#if WITH_EDITORONLY_DATA
	// Arrow indicating the direction of the impulse
	UPROPERTY(Category = "Components", EditAnywhere)
		UArrowComponent* DirectionArrow;
	// Arrow indicating the required orientation
	UPROPERTY(Category = "Components", EditAnywhere)
		UArrowComponent* OrientationArrow;
#endif
};
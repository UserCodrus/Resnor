// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DamageTrigger.generated.h"

#if WITH_EDITORONLY_DATA
class UArrowComponent;
#endif

UCLASS()
class CYBERSHOOTER_API ADamageTrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	ADamageTrigger();

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

public:
	/// Damage Trigger Functions ///

	// Damage actors inside the trigger
	UFUNCTION()
		void BeginOverlap(AActor* OverlappedActor, AActor* OtherActor);
	// Stop damaging actors that leave the trigger
	UFUNCTION()
		void EndOverlap(AActor* OverlappedActor, AActor* OtherActor);

	// Damage the trigger's current targets
	UFUNCTION(BlueprintCallable)
		void ApplyDamage();
	// Activate collision
	UFUNCTION(BlueprintCallable)
		void ActivateTrigger();
	// Deactivate collision
	UFUNCTION(BlueprintCallable)
		void DeactivateTrigger();

	// Disable the damage trigger
	UFUNCTION(BlueprintCallable)
		void DisableTrigger();
	// Enable the damage trigger
	UFUNCTION(BlueprintCallable)
		void EnableTrigger();

	/// Blueprint Events ///
	
	// Called when the trigger activates
	UFUNCTION(BlueprintImplementableEvent)
		void Activate();
	// Called when the trigger deactivates
	UFUNCTION(BlueprintImplementableEvent)
		void Deactivate();

protected:
	/// Properties ///
	
	// The damage that actors will take when inside the trigger
	UPROPERTY(Category = "Damage", EditAnywhere)
		int32 Damage;
	// The force feedback from impacts
	UPROPERTY(Category = "Damage", EditAnywhere)
		UForceFeedbackEffect* RumbleEffect;

	// If set to true, the trigger will deal damage once when activated, otherwise it will deal damage continuously while Active is set to true
	UPROPERTY(Category = "Damage", EditAnywhere)
		bool InstantDamage;
	// Set to true when the trigger is dealing damage
	UPROPERTY(Category = "Damage", EditAnywhere, BlueprintReadOnly)
		bool Active;
	// Set to true when the trigger is able to deal damage and cycle itself
	UPROPERTY(Category = "Damage", EditAnywhere, BlueprintReadOnly)
		bool Disabled;
	// The force applied by the trigger when a pawn hits it
	UPROPERTY(Category = "Damage", EditAnywhere)
		float Impulse;

	// If set to true, the trigger will target pawns inside its collision, otherwise targets must be set manually
	UPROPERTY(Category = "Targets", EditAnywhere)
		bool TargetOverlap;
	// The trigger's target, set automatically if TargetOverlap is set to true, otherwise they must be set manually
	UPROPERTY(Category = "Targets", EditInstanceOnly, BlueprintReadOnly, meta = (MustImplement = "DamageInterface"))
		TArray<AActor*> Targets;

	// If set to true, the trigger will cycle between the active and inactive state over time, otherwise it must be activated with a lock
	UPROPERTY(Category = "Timing", EditAnywhere)
		bool AutoCycle;
	// The amount of time in seconds that the trigger will be active when AutoCycle is enabled, if InstantDamage is enabled this will be ignored
	UPROPERTY(Category = "Timing", EditAnywhere)
		float ActiveDuration;
	// The amount of time in seconds that the trigger will be dormant when AutoCycle is enabled
	UPROPERTY(Category = "Timing", EditAnywhere)
		float InactiveDuration;
	// The offset that the timer starts at to allow alternating patters in a level
	UPROPERTY(Category = "Timing", EditAnywhere)
		float TimerOffset;

	// The component that controls the direction of the force
	UPROPERTY(Category = "Components", EditAnywhere)
		USceneComponent* DirectionComponent;

#if WITH_EDITORONLY_DATA
	// Arrow indicating the physics direction
	UPROPERTY(Category = "Components", EditAnywhere)
		UArrowComponent* DirectionArrow;
#endif

	// The timer for managing the trigger
	float Timer;
};

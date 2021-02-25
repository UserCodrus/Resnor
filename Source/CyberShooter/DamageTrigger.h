// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "LockInterface.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DamageTrigger.generated.h"

UCLASS()
class CYBERSHOOTER_API ADamageTrigger : public AActor, public ILockInterface
{
	GENERATED_BODY()
	
public:	
	ADamageTrigger();

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

public:
	/// ILockInterface ///

	void Unlock() override;
	void Lock() override;

	/// Damage Trigger Functions ///

	// Damage actors inside the trigger
	UFUNCTION()
		void BeginOverlap(AActor* OverlappedActor, AActor* OtherActor);
	// Stop damaging actors that leave the trigger
	UFUNCTION()
		void EndOverlap(AActor* OverlappedActor, AActor* OtherActor);

	// Damage the trigger's current targets
	void ApplyDamage();

protected:
	// Activate collision
	void ActivateTrigger();
	// Deactivate collision
	void DeactivateTrigger();

	// Called when the trigger activates
	UFUNCTION(BlueprintImplementableEvent)
		void OnActivate();
	// Called when the trigger deactivates
	UFUNCTION(BlueprintImplementableEvent)
		void OnDeactivate();

	// The damage that actors will take when inside the trigger
	UPROPERTY(Category = "Damage", EditAnywhere)
		int32 Damage;
	// The force feedback from impacts
	UPROPERTY(Category = "Damage", EditAnywhere)
		UForceFeedbackEffect* RumbleEffect;

	// If set to true, the trigger will deal damage once when activated, otherwise it will deal damage continuously while DealingDamage is set to true
	UPROPERTY(Category = "Damage", EditAnywhere)
		bool InstantDamage;
	// Set to true when the trigger is dealing damage
	UPROPERTY(Category = "Damage", EditAnywhere)
		bool Active;

	// Set to true when the trigger is enabled
	UPROPERTY(Category = "Locking", EditAnywhere)
		bool Disabled;
	// If set to true, the trigger will enabled or disabled when unlocked or locked, otherwise it will activate when unlocked
	UPROPERTY(Category = "Locking", EditAnywhere)
		bool EnableWhenUnlocked;

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

	// The timer for managing the trigger
	float Timer;
};

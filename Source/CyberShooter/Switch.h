// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "CombatInterface.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Switch.generated.h"

class ALock;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSwitchEvent);

// A switch that can trigger a lock
UCLASS(Abstract)
class CYBERSHOOTER_API ASwitch : public AActor
{
	GENERATED_BODY()
	
public:	
	ASwitch();

	// Called when the switch timer expires after the switch is untriggered
	void TriggerTimeout();
	// Called when the switch's cooldown runs out
	void CooldownTimeout();

	// Call the activation delegate for the switch and its parents
	void ActivateEvent();
	// Call the deactivation delegate for the switch and its parents
	void DeactivateEvent();

	/// Blueprint Events ///

	// Called when the switch or its child is pressed
	UPROPERTY(BlueprintAssignable, Category = "Events")
		FSwitchEvent OnActivate;
	// Called when the switch or its child is released
	UPROPERTY(BlueprintAssignable, Category = "Events")
		FSwitchEvent OnDeactivate;

	// Called when this switch is activated
	UFUNCTION(BlueprintImplementableEvent)
		void Trigger();
	// Called when this switch is deactivated
	UFUNCTION(BlueprintImplementableEvent)
		void Release();

	/// Accessors ///
	// Returns true if the switch has been activated
	UFUNCTION(BlueprintPure)
		bool IsTriggered() const { return Triggered; }

protected:
	/// Switch Functions ///

	// Trigger a lock when the switch activates
	UFUNCTION()
		void BeginOverlap(AActor* OverlappedActor, AActor* OtherActor);
	// Reverse a trigger when the switch deactivates
	UFUNCTION()
		void EndOverlap(AActor* OverlappedActor, AActor* OtherActor);

	// Determine if a given actor can trigger this switch
	virtual float GetTriggerValue(AActor* TriggeringActor);
	// Activate the switch
	void TriggerSwitch();
	// Deactivate the switch
	void ReleaseSwitch();

	/// Properties ///

	// If set to true, the switch will stay on permanently after triggered, ignoring TriggerDuration
	// Damage switches can be toggled on or off if this is set to false, but will still set timers if TriggerDuration is > 0
	UPROPERTY(Category = "Switch|Trigger", EditAnywhere)
		bool PermanentTrigger;
	// The amount of time the switch will stay triggered after being pressed
	// Damage switches will always switch off after this duration unless set to 0, other switches will ignore this if PermanentTigger is true
	UPROPERTY(Category = "Switch|Trigger", EditAnywhere)
		float TriggerDuration;
	// The amount of time that must pass before the switch can be triggered again
	// This must be shorter than TriggerDuration
	UPROPERTY(Category = "Switch|Trigger", EditAnywhere)
		float CooldownDuration;
	// The total pressure on the switch, used to determine if the switch will trigger when an actor steps on to it
	UPROPERTY(Category = "Switch|Trigger", VisibleAnywhere)
		float TotalWeight;
	// The pressure required to trigger the switch
	UPROPERTY(Category = "Switch|Trigger", EditAnywhere)
		float RequiredWeight;

	// Set to true after the switch has been activated
	UPROPERTY(VisibleAnywhere)
		bool Triggered;
	// Set to true when the switch is on cooldown
	UPROPERTY(VisibleAnywhere)
		bool Cooldown;

	// The sound played when the switch triggers
	UPROPERTY(Category = "Switch|Sound", EditAnywhere)
		USoundBase* TriggerSound;
	// The sound played when the switch untriggers
	UPROPERTY(Category = "Switch|Sound", EditAnywhere)
		USoundBase* UntriggerSound;

	// The timer handle for the trigger timer
	FTimerHandle TimerHandle_SwitchTimer;
	// The timer handle for cooldowns
	FTimerHandle TimerHandle_SwitchCooldown;
};

// A switch that can be triggered by overlapping a player
// The weight of the switch is one higher than the number of keys the player has collected
UCLASS(BlueprintType, Blueprintable)
class CYBERSHOOTER_API ASwitch_Player : public ASwitch
{
	GENERATED_BODY()

public:
	ASwitch_Player();

protected:
	virtual float GetTriggerValue(AActor* TriggeringActor);
};

// A switch that can be triggered by overlapping any physics object
// The weight value on the switch is equal to the physics weight of each object on the switch
UCLASS(BlueprintType, Blueprintable)
class CYBERSHOOTER_API ASwitch_Physics : public ASwitch
{
	GENERATED_BODY()

public:
	ASwitch_Physics();

protected:
	virtual float GetTriggerValue(AActor* TriggeringActor);
};

// A switch that is triggered when it takes damage
// The weight on the switch is equal to the damage the switch takes
UCLASS(BlueprintType, Blueprintable)
class CYBERSHOOTER_API ASwitch_Damage : public ASwitch, public ICombatInterface
{
	GENERATED_BODY()

public:
	ASwitch_Damage();

	/// ICombatInterface ///

	virtual bool Damage(int32 Value, int32 DamageType, UForceFeedbackEffect* RumbleEffect, UPrimitiveComponent* HitComp = nullptr, AActor* Source = nullptr, AActor* Origin = nullptr) override;
	virtual void Heal(int32 Value) override;
	virtual void Kill() override;

protected:
	virtual float GetTriggerValue(AActor* TriggeringActor);

	// Damage types that won't trigger the switch
	UPROPERTY(Category = "Switch|Trigger", EditAnywhere, meta = (Bitmask, BitmaskEnum = EDamageTypeFlag))
		int32 DamageImmunity;
};
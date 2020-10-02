// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Lock.generated.h"

// An actor that can control doors and other gadgets by locking and unlocking via switches
UCLASS(BlueprintType, Blueprintable)
class CYBERSHOOTER_API ALock : public AActor
{
	GENERATED_BODY()
	
public:	
	ALock();

	/// Lock Functions ///

	// Returns true if the lock is currently locked
	UFUNCTION(BlueprintPure)
		bool IsLocked() const { return Locked; }

	// Called when the lock is triggered by a switch or other gadget
	UFUNCTION(BlueprintCallable)
		bool TriggerLock();
	// Called when a trigger is reversed
	UFUNCTION(BlueprintCallable)
		bool UntriggerLock();

protected:
	/// Blueprint Events ///
	
	// Called when the lock is unlocked
	UFUNCTION(BlueprintNativeEvent)
		void OnUnlock();
	// Called when the lock is re-locked
	UFUNCTION(BlueprintNativeEvent)
		void OnLock();

	/// Properties ///
	
	// The target actor that is activated by this lock
	UPROPERTY(Category = "Lock", EditInstanceOnly, BlueprintReadOnly, meta = (MustImplement = "LockInterface"))
		AActor* Target;

	// The number of triggers used on the lock so far
	UPROPERTY(Category = "Lock", EditAnywhere, BlueprintReadOnly)
		int32 NumTriggers;
	// The number of triggers required to open this lock
	UPROPERTY(Category = "Lock", EditAnywhere, BlueprintReadOnly)
		int32 RequiredTriggers;
	// If set to true, the lock can not be unlocked after being locked
	UPROPERTY(Category = "Lock", EditAnywhere, BlueprintReadOnly)
		bool PermanentUnlock;

	// Set to true when the lock is locked
	UPROPERTY(VisibleAnywhere)
		bool Locked;
};

// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "LockInterface.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PowerUp.generated.h"

UCLASS()
class CYBERSHOOTER_API APowerUp : public AActor, public ILockInterface
{
	GENERATED_BODY()
	
public:	
	APowerUp();

	// Collect the powerup when overlapping the player
	UFUNCTION()
		void BeginOverlap(AActor* OverlappedActor, AActor* OtherActor);
	// The event called when collecting the powerup
	UFUNCTION(BlueprintImplementableEvent)
		void Collect(class ACyberShooterPlayer* Player);

	// Disable the power up
	UFUNCTION(BlueprintCallable)
		void Disable();

	// Respawn the powerup
	void Respawn();

	/// ILockInterface ///

	void Unlock() override;
	void Lock() override;

protected:
	// Set to true when the powerup can be collected
	UPROPERTY(VisibleAnywhere)
		bool Active;

	// The lock that this collectible triggers
	UPROPERTY(Category = "Trigger", EditInstanceOnly)
		class ALock* Target;
	
	// The sound played when collecting the powerup
	UPROPERTY(Category = "Settings", EditAnywhere)
		USoundBase* CollectSound;
	// The time it takes the powerup to respawn
	UPROPERTY(Category = "Settings", EditAnywhere)
		float RespawnDuration;
	// If set to true, the powerup can only be collected if the player matches its orientation
	UPROPERTY(Category = "Settings", EditAnywhere)
		bool RestrictOrientation;

	// The timer for managing respawns
	FTimerHandle TimerHandle_RespawnTimer;
	// Set to true when the powerup triggers a lock
	bool HasTriggered;
};

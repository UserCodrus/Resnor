// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PowerUp.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCollectibleEvent);

// A collectible or upgrade the the player can collect
UCLASS()
class CYBERSHOOTER_API APowerUp : public AActor
{
	GENERATED_BODY()
	
public:	
	APowerUp();

	virtual void BeginPlay() override;

	// Collect the powerup when overlapping the player
	UFUNCTION()
		void BeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

	// Disable the power up
	UFUNCTION(BlueprintCallable)
		void Disable();
	// Respawn the powerup
	UFUNCTION(BlueprintCallable)
		void Respawn();

	// Broadcast the collect event for this powerup and its parents
	UFUNCTION(BlueprintCallable)
		void TriggerCollectEvent();
	// Broadcast the respawn event for this powerup and its parents
	UFUNCTION(BlueprintCallable)
		void TriggerRespawnEvent();

	/// Blueprint Events ///

	// Called when the switch or its child is pressed
	UPROPERTY(BlueprintAssignable, Category = "Events")
		FCollectibleEvent OnCollect;
	// Called when the switch or its child is released
	UPROPERTY(BlueprintAssignable, Category = "Events")
		FCollectibleEvent OnRespawn;

	// The event called when collecting the powerup
	UFUNCTION(BlueprintImplementableEvent)
		void Collect(class ACyberShooterPlayer* Player);

protected:
	/// Properties ///

	// Set to true when the powerup can be collected
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool Active;
	
	// The time it takes the powerup to respawn, if set to 0 the powerup won't respawn on its own
	UPROPERTY(Category = "Settings", EditAnywhere)
		float RespawnDuration;
	// If set to false, the powerup will never respawn
	UPROPERTY(Category = "Settings", EditAnywhere, BlueprintReadWrite)
		bool CanRespawn;
	// If set to true, the powerup can only be collected if the player matches its orientation
	UPROPERTY(Category = "Settings", EditAnywhere)
		bool RestrictOrientation;

	// The sound played when collecting the powerup
	UPROPERTY(Category = "Settings", EditAnywhere)
		USoundBase* CollectSound;

	// The timer for managing respawns
	FTimerHandle TimerHandle_RespawnTimer;
};

// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "AggroInterface.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AggroZone.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAggroEvent);

// A box that controls the aggro of enemies inside it
UCLASS()
class CYBERSHOOTER_API AAggroZone : public AActor
{
	GENERATED_BODY()
	
public:	
	AAggroZone();

	virtual void BeginPlay() override;

	// Aggro enemies when the player enters the zone
	UFUNCTION()
		void BeginOverlap(AActor* OverlappedActor, AActor* OtherActor);
	// Remove aggro when the player exits the zone
	UFUNCTION()
		void EndOverlap(AActor* OverlappedActor, AActor* OtherActor);

	// Update the list of enemies to include all enemies in this zone
	UFUNCTION(Category = "AI", CallInEditor)
		void UpdateActorList();

	// Notify the zone that an enemy has been registered
	void NotifyRegister();
	// Notify the zone that one of its actors has despawned
	void NotifyDespawn();

	// Force the zone to respawn
	UFUNCTION(BlueprintCallable)
	void Respawn();
	// Disable the aggro zone
	UFUNCTION(BlueprintCallable)
	void Disable();

	// Activate the aggro zone
	UFUNCTION(BlueprintCallable)
		void Activate();
	// Disable the aggro zone
	UFUNCTION(BlueprintCallable)
		void Deactivate();

	/// Blueprint Events ///

	UPROPERTY(BlueprintAssignable, Category = "Events")
		FAggroEvent OnEnter;
	UPROPERTY(BlueprintAssignable, Category = "Events")
		FAggroEvent OnExit;
	UPROPERTY(BlueprintAssignable, Category = "Events")
		FAggroEvent OnClear;
	UPROPERTY(BlueprintAssignable, Category = "Events")
		FAggroEvent OnRespawn;

protected:
	/// Properties ///

	// Actors controlled by this zone
	UPROPERTY(Category = "AI", EditInstanceOnly, meta = (MustImplement = "AggroInterface"))
		TArray<AActor*> Actors;

	// Disable the enemies in the zone when the zone is inactive
	UPROPERTY(Category = "AI", EditAnywhere)
		bool DisableActors;
	// If set to true, the player will only trigger the zone if its orientation is the same as the zone
	UPROPERTY(Category = "AI", EditAnywhere)
		bool RestrictOrientation;
	// If set to true, enemies will respawn if they leave this zone and will respawn after being killed
	UPROPERTY(Category = "AI", EditAnywhere)
		bool RespawnActors;
	// The time it takes to respawn enemies after the player leaves the zone
	UPROPERTY(Category = "AI", EditAnywhere)
		float RespawnTime;

	// If set to false, the zone will not activate when the player enters it
	UPROPERTY(Category = "Status", EditAnywhere, BlueprintReadOnly)
		bool Active;
	// Set to true after the zone is cleared for the first time
	UPROPERTY(Category = "Status", EditAnywhere, BlueprintReadOnly)
		bool Cleared;
	// Set to true when the player has activated the aggro zone
	UPROPERTY(Category = "Status", EditAnywhere, BlueprintReadOnly)
		bool Aggro;

	// If set to true, the zone will change the camera distance when the player enters it
	UPROPERTY(Category = "Camera", EditAnywhere)
		bool SetCamera;
	// The camera distance in the zone
	UPROPERTY(Category = "Camera", EditAnywhere)
		float CameraDistance;

	// The number of enemies that this zone is tracking
	UPROPERTY(EditAnywhere)
		int32 TotalEnemies;
	// The number of tracked enemies that have despawned
	UPROPERTY(EditAnywhere)
		int32 DespawnedEnemies;

	// The collision box for handling overlaps
	UPROPERTY(Category = "Components", EditAnywhere)
		class UBoxComponent* CollisionBox;

	// The sound played when the zone is cleared
	UPROPERTY(Category = "Sound", EditAnywhere)
		USoundBase* ClearSound;

	// The timer handle for enemy respawns
	FTimerHandle TimerHandle_RespawnTimer;

#if WITH_EDITORONLY_DATA
	// Arrow indicating the orientation of the zone
	UPROPERTY(Category = "Components", EditAnywhere)
		class UArrowComponent* OrientationArrow;
#endif
};

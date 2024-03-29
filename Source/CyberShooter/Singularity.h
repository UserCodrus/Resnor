// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Singularity.generated.h"

class IPhysicsInterface;
class ACyberShooterProjectile;

// A gadget that pulls objects and projectiles within its bounds towards its center
UCLASS()
class CYBERSHOOTER_API ASingularity : public AActor
{
	GENERATED_BODY()
	
public:	
	ASingularity();

	// Called when a pawn enters the singularity to apply force
	UFUNCTION()
		void BeginOverlap(AActor* OverlappedActor, AActor* OtherActor);
	// Called when a pawn leaves the sinularity to remove static forces
	UFUNCTION()
		void EndOverlap(AActor* OverlappedActor, AActor* OtherActor);

	virtual void Tick(float DeltaTime) override;

protected:
	// The force that the singularity applies
	UPROPERTY(Category = "Physics", EditAnywhere)
		float Force;

	// Objects inside the singularity
	UPROPERTY(VisibleAnywhere)
		TArray<AActor*> Objects;
	// Projectiles inside the singularity
	UPROPERTY(VisibleAnywhere)
		TArray<ACyberShooterProjectile*> Projectiles;

	// The collision capsule
	UPROPERTY(Category = "Components", VisibleDefaultsOnly, BlueprintReadOnly)
		class USphereComponent* CollisionComponent;
};

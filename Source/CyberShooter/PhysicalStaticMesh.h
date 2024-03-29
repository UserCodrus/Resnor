// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "PhysicalStaticMesh.generated.h"

// A static mesh actor with physics settings
UCLASS()
class CYBERSHOOTER_API APhysicalStaticMesh : public AStaticMeshActor
{
	GENERATED_BODY()

public:
	APhysicalStaticMesh();

	virtual FVector GetVelocity() const override;

	// Returns true if the mesh is a static object
	virtual bool IsStable() const;

	FORCEINLINE float GetFriction() { return Friction; }
	FORCEINLINE float GetDamage() { return Damage; }

protected:
	// The surface friction of the object
	UPROPERTY(Category = "PhysicsProperties|Surface", EditAnywhere)
		float Friction;
	// The damage the floor does to pawns standing on it
	UPROPERTY(Category = "PhysicsProperties|Surface", EditAnywhere)
		int32 Damage;
	// The conveyor effect on objects standing on the mesh
	UPROPERTY(Category = "PhysicsProperties|Surface", EditAnywhere)
		FVector FloorVelocity;
	// If set to true, the player will be able to respawn here
	UPROPERTY(Category = "PhysicsProperties|Surface", EditAnywhere)
		bool Safe;
};

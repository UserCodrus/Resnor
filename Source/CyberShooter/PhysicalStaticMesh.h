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

	FORCEINLINE float GetFriction() { return Friction; }
	FORCEINLINE float GetDamage() { return Damage; }

protected:
	// The surface friction of the object
	UPROPERTY(Category = Surface, EditAnywhere)
		float Friction;
	// The damage the floor does to pawns standing on it
	UPROPERTY(Category = Surface, EditAnywhere)
		float Damage;
};

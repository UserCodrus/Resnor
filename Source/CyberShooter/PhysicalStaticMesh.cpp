// Copyright © 2020 Brian Faubion. All rights reserved.

#include "PhysicalStaticMesh.h"

APhysicalStaticMesh::APhysicalStaticMesh()
{
	Friction = 1.0f;
	Damage = 0.0f;
	FloorVelocity = FVector::ZeroVector;
}

FVector APhysicalStaticMesh::GetVelocity() const
{
	return FloorVelocity;
}

bool APhysicalStaticMesh::IsStable() const
{
	return GetStaticMeshComponent()->Mobility == EComponentMobility::Static;
}
// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "EnemyAIController.h"

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyTurretAI.generated.h"

class AEnemyTurret;

// Enemy AI that sits in place and aims at nearby players
UCLASS()
class CYBERSHOOTER_API AEnemyTurretAI : public AEnemyAIController
{
	GENERATED_BODY()
	
public:
	AEnemyTurretAI();

	virtual void Tick(float DeltaSeconds) override;
};

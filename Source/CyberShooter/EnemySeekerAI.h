// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "EnemyAIController.h"

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemySeekerAI.generated.h"

class AEnemySeeker;

// Enemy AI that chases after players
UCLASS()
class CYBERSHOOTER_API AEnemySeekerAI : public AEnemyAIController
{
	GENERATED_BODY()
	
public:
	AEnemySeekerAI();

	virtual void Tick(float DeltaSeconds) override;
};

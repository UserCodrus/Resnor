// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

class ACyberShooterPlayer;
class AEnemyBase;

// The base controller for enemy AI
UCLASS()
class CYBERSHOOTER_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	
	// Set to true when the player is close enough to activate the AI
	UPROPERTY(VisibleAnywhere)
		bool Aggro;

protected:
	// The player character
	UPROPERTY(VisibleAnywhere)
		ACyberShooterPlayer* Player;
};

// Copyright © 2020 Brian Faubion. All rights reserved.

#include "CyberShooterGameInstance.h"

UCyberShooterGameInstance::UCyberShooterGameInstance()
{
	Gravity = 1000.0f;
	AirFriction = 0.5f;
}

UAbilityScript* UCyberShooterGameInstance::GetScript(TSubclassOf<UAbilityScript> ScriptType)
{
	// Check for existing scripts
	for (int32 i = 0; i < Scripts.Num(); ++i)
	{
		if (Scripts[i]->StaticClass() == ScriptType)
		{
			return Scripts[i];
		}
	}

	// Create a new script if one does not exist
	Scripts.Add(NewObject<UAbilityScript>(this, ScriptType));
	return Scripts.Last();
}
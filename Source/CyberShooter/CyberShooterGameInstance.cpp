// Copyright © 2020 Brian Faubion. All rights reserved.

#include "CyberShooterGameInstance.h"

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
// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class CyberShooter : ModuleRules
{
	public CyberShooter(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "UMG" });
	}
}

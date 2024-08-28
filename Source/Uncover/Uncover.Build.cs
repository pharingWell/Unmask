// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Uncover : ModuleRules
{
	public Uncover(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });
	}
}

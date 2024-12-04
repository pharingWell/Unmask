// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Unmask : ModuleRules
{
	public Unmask(ReadOnlyTargetRules Target) : base(Target)
	{
		PrivateDependencyModuleNames.AddRange(new string[] { "AIModule", "Niagara", "Engine"} );
		
		if (Target.bBuildEditor)
		{
			PublicDependencyModuleNames.AddRange(new string[]{"UnrealEd",});
		}
		
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "AnimationCore"});
	}
}

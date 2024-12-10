// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class EditorModule : ModuleRules
{
	public EditorModule(ReadOnlyTargetRules Target) : base(Target)
	{
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "Engine", "UnrealEd", });
	}
}

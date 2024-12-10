// Fill out your copyright notice in the Description page of Project Settings.
#if WITH_EDITOR

#include "MyEdEngine.h"
#include "ISourceControlModule.h"
#include "ISourceControlProvider.h"

void UMyEdEngine::Init(IEngineLoop* InEngineLoop)
{
	Super::Init(InEngineLoop);

	// Register state branches
	const ISourceControlModule& SourceControlModule = ISourceControlModule::Get();
	{
		ISourceControlProvider& SourceControlProvider = SourceControlModule.GetProvider();
		// Order matters. Lower values are lower in the hierarchy, i.e., changes from higher branches get automatically merged down.
		// (Automatic merging requires an appropriately configured CI pipeline)
		// With this paradigm, the higher the branch is, the stabler it is, and has changes manually promoted up.
		const TArray<FString> Branches {"origin/develop", "origin/promoted"};
		SourceControlProvider.RegisterStateBranches(Branches, TEXT("Content"));
	}
}
#endif


// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimData/UMSequenceStructs.h"
#include "Components/PoseableMeshComponent.h"
#include "UObject/Object.h"
#include "UMAnimationRecorder.generated.h"

/**
 * 
 */
USTRUCT()
struct FParentChildPair
{
	GENERATED_BODY()
	int32 ParentIndex, ChildIndex;
};

UCLASS(BlueprintType)
class UNMASK_API UUMAnimationRecorder : public UObject
{
	GENERATED_BODY()
protected:
	USkeletalMeshComponent* SkeletalMeshComponent;
	FUMJointGroup RootGroup;
	TMap<FName, FUMJointGroup> AllGroups;

public:
	UUMAnimationRecorder(): RootGroup("Root") { this->SkeletalMeshComponent = nullptr; }
	UFUNCTION(BlueprintCallable)
	static UUMAnimationRecorder* GetNewAnimationRecorder();
	UFUNCTION(BlueprintCallable)
	void InitAnimationRecorder(USkeletalMeshComponent* InSkeletalMeshComponent);
	UFUNCTION(BlueprintCallable)
	FName GetGroupWithBone(FName Bone);
	UFUNCTION(BlueprintCallable)
	void HideAllButGroup(FName GroupName);
	FString AllGroupsToString();
	UFUNCTION(BlueprintCallable)
	void PrintAllGroups();
	UFUNCTION(BlueprintCallable)
	void LoadTimelines(TMap<FName, FUMJointTimeline> Timelines);
	UFUNCTION(BluePrintCallable)
	FUMJointGroup GetRootGroup() { return this->RootGroup; }
	UFUNCTION(BluePrintCallable)
	TMap<FName, FUMJointGroup> GetGroups() { return this->AllGroups;}
private:
	bool IsInGroup(FName BoneName, FName GroupName, bool bIncludeChildren);
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimData/UMSequenceStructs.h"
#include "Components/PoseableMeshComponent.h"
#include "Templates/Tuple.h"
#include "UObject/Object.h"
#include "UMAnimationRecorder.generated.h"

/**
 * 
 */

template<typename... Types>  struct TTuple;

USTRUCT()
struct FUMParentChildPair
{
	GENERATED_BODY()
	int32 ParentIndex, ChildIndex;
};

UENUM()
enum EUMAnimEditor_MatSlot
{
	LEG_L = 0,
	ARM_L,
	LEG_R,
	ARM_R,
	HEAD,
	TORSO
};

UCLASS(BlueprintType)
class UNMASK_API UUMAnimationRecorder : public UObject
{
	GENERATED_BODY()
protected:
	UPROPERTY()
	USkeletalMeshComponent* SkeletalMeshComponent;
	UPROPERTY()
	UUMJointGroup* RootGroup;
	UPROPERTY()
	TMap<FName, UUMJointGroup*> AllGroups;
	UPROPERTY()
	TArray<UMaterialInstanceDynamic*> DynMatInsts;

public:
	UUMAnimationRecorder();
	UFUNCTION(BlueprintCallable)
	static UUMAnimationRecorder* GetNewAnimationRecorder();
	UFUNCTION(BlueprintCallable)
	void InitAnimationRecorder(USkeletalMeshComponent* InSkeletalMeshComponent);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UUMJointGroup* GetGroupWithBone(FName Bone);
	UFUNCTION(BlueprintCallable)
	void SelectGroup(UUMJointGroup* Group);
	FString AllGroupsToString();
	UFUNCTION(BlueprintCallable)
	void PrintAllGroups();
	UFUNCTION(BlueprintCallable)
	void LoadTimelines(TMap<FName, FUMJointTimeline> Timelines);
	UFUNCTION(BluePrintCallable)
	UUMJointGroup* GetRootGroup() { return this->RootGroup; }
	UFUNCTION(BluePrintCallable)
	TMap<FName, UUMJointGroup*> GetGroups() { return this->AllGroups;}
};
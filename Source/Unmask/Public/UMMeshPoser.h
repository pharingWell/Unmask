// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/UMSequenceStructs.h"
#include "Components/PoseableMeshComponent.h"
#include "UObject/Object.h"
#include "UMMeshPoser.generated.h"

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
class UNMASK_API UUMMeshPoser : public UObject
{
	GENERATED_BODY()
protected:
	UPoseableMeshComponent* PoseableMesh;
	FUMJointGroup RootGroup;
	TMap<FName, FUMJointGroup> AllGroups;

public:
	UUMMeshPoser() { this->PoseableMesh = nullptr; }
	UFUNCTION(BlueprintCallable)
	static UUMMeshPoser* GetNewMeshPoser();
	UFUNCTION(BlueprintCallable)
	void InitMeshPoser(UPoseableMeshComponent* InSkeletalMesh);
	UFUNCTION(BlueprintCallable)
	void SetBoneTransform(FName Bone, FTransform Transform) const;
	UFUNCTION(BlueprintCallable)
	void HideAllButGroup(FName GroupName);
private:
	bool IsInGroup(FName BoneName, FName GroupName);
};

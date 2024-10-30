// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UMCueTree.generated.h"
#include "Animation/UMSequenceStructs.h"
#include "Animation/UMAnimationProducer.h"

USTRUCT(Blueprintable, BlueprintType)
struct FCueTreeNode
{
	GENERATED_BODY()
	
	UPROPERTY(Blueprintable, BlueprintReadOnly)
	UAnimSequence *Animation;
	UPROPERTY(Blueprintable, BlueprintReadOnly)
	TArray<int> Children;
	UPROPERTY(Blueprintable, BlueprintReadOnly)
	int Depth;
};

USTRUCT(BlueprintType, BlueprintType)
struct FBoneRotatorRange
{
	GENERATED_BODY()

	UPROPERTY(Blueprintable, BlueprintReadWrite)
	FRotator RotatorMin;
	UPROPERTY(Blueprintable, BlueprintReadWrite)
	FRotator RotatorMax;
};

UCLASS()
class UNMASK_API UUMCueTree : public UObject
{
	GENERATED_BODY()
	
	protected:
		UPROPERTY()
		TArray<FCueTreeNode> Nodes;
		UPROPERTY()
		USkeletalMesh *SkeletalMesh;
		UFUNCTION()
		UAnimSequence *UUMCueTree::GenerateAnimation(USkeletalMesh *SkeletalMesh, const TMap<FName, FBoneRotatorRange> *Ranges, const int Frames, const float PlayLength);
	
	UFUNCTION(BlueprintCallable, Category = "CueTree")
	UUMCueTree::UUMCueTree(USkeletalMesh *SkeletalMesh, const TMap<FName, FBoneRotatorRange> *Ranges, const int AvgChildren, const int MinDepth, const int MaxDepth, const int Frames, const float PlayLength);
	UFUNCTION(BlueprintCallable, Category = "CueTree")
	FCueTreeNode *GetRoot();
	UFUNCTION(BlueprintCallable, Category = "CueTree")
	static float RandInRange(float Min, float Max);
};

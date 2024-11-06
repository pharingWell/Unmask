// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Animation/UMSequenceStructs.h"
#include "Animation/UMAnimationProducer.h"
#include "UMCueTree.generated.h"

USTRUCT(Blueprintable, BlueprintType)
struct FCueTreeNode
{
	GENERATED_BODY()
	
	UPROPERTY(Blueprintable, BlueprintReadOnly)
	UAnimSequence *Animation {nullptr};
	UPROPERTY(Blueprintable, BlueprintReadOnly)
	TArray<int> Children;
	UPROPERTY(Blueprintable, BlueprintReadOnly)
	int Depth {-1};
};

USTRUCT(BlueprintType, BlueprintType)
struct FBoneRotatorRange
{
	GENERATED_BODY()

	UPROPERTY(Blueprintable, BlueprintReadWrite)
	FRotator RotatorMin {FRotator()};
	UPROPERTY(Blueprintable, BlueprintReadWrite)
	FRotator RotatorMax {FRotator()};
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
		UPROPERTY()
		TMap<FName, FBoneRotatorRange> Ranges;
		UFUNCTION()
		UAnimSequence *GenerateAnimation(const int Frames, const float PlayLength);

	UUMCueTree();
	UUMCueTree(USkeletalMesh *SkeletalMesh, const TMap<FName, FBoneRotatorRange> Ranges, const int AvgChildren, const int MinDepth, const int MaxDepth, const int Frames, const float PlayLength);
	UFUNCTION(BlueprintCallable, Category = "CueTree")
	FCueTreeNode GetRoot();
	UFUNCTION(BlueprintCallable, Category = "CueTree")
	static float RandInRange(float Min, float Max);
};

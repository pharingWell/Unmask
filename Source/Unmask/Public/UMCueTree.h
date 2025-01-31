// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Animation/AnimData/UMSequenceStructs.h"
#include "Animation/UMAnimationProducer.h"
#include "UMCueTree.generated.h"

USTRUCT(Blueprintable, BlueprintType)
struct FUMCueTreeNode
{
	GENERATED_BODY()
	
	UPROPERTY(Blueprintable, BlueprintReadOnly)
	UAnimSequence *Animation {nullptr};
	UPROPERTY(Blueprintable, BlueprintReadOnly)
	TArray<int> Children;
	UPROPERTY(Blueprintable, BlueprintReadOnly)
	int Depth {-1};
};

UCLASS()
class UNMASK_API UUMCueTree : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(Blueprintable, BlueprintReadOnly)
	TArray<FUMCueTreeNode> Nodes;
protected:
	UPROPERTY()
	USkeletalMesh *SkeletalMesh;
	UPROPERTY()
	TMap<FName, FRotatorRange> Ranges;
	UFUNCTION()
	UAnimSequence *GenerateAnimation(const int Frames, const float PlayLength);

	UUMCueTree();
	void InitCueTree(USkeletalMesh *InitSkeletalMesh, const TMap<FName, FRotatorRange> InitRanges, const int AvgChildren, const int MinDepth, const int MaxDepth, const int Frames, const float PlayLength);
	UFUNCTION(BlueprintPure, Category = "CueTree")
	FUMCueTreeNode GetRoot() { return Nodes[0]; }
	UFUNCTION(BlueprintPure, Category = "CueTree")
	static float RandInRange(float Min, float Max);
	UFUNCTION(BlueprintCallable, Category = "CueTree")
	static UUMCueTree *CreateMVPTree(USkeletalMesh *Mesh);
	
};

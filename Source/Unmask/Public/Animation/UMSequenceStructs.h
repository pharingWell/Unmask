#pragma once

#include "UMSequenceStructs.generated.h"

USTRUCT(Blueprintable, BlueprintType)
struct FUMKeyFrame
{
	GENERATED_BODY()

public:
	UPROPERTY(Blueprintable, BlueprintReadWrite)
	float Time = 0.f;
	
	UPROPERTY(Blueprintable, BlueprintReadWrite)
	FTransform Transform = {};

};

USTRUCT(Blueprintable, BlueprintType)
struct FUMJointSequence
{
	GENERATED_BODY()
public:
	UPROPERTY(Blueprintable, BlueprintReadWrite)
	TArray<FUMKeyFrame> JointSequence = {};

};

UCLASS()
class UMSequenceHelper :  public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintPure, Category = "Animation|Sequencer", meta=(BlueprintThreadSafe))
	static FUMKeyFrame MakeKeyframe(float Time, FTransform Transform);

	UFUNCTION(BlueprintPure, Category = "Animation|Sequencer", meta=(BlueprintThreadSafe))
	static FUMJointSequence MakeJointSequence(const TArray<FUMKeyFrame>& Joints);
};
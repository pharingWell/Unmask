#pragma once

USTRUCT()
struct FUMKeyFrame
{
	GENERATED_BODY()
	float Time;
	FTransform Transform;
};

USTRUCT()
struct FUMJointSequence
{
	GENERATED_BODY()
	// float: Current Time, FTransform: joint transform
	TArray<FUMKeyFrame> JointSequence;
	
};
#pragma once

#include "UMSequenceStructs.generated.h"

class UUMSequenceHelper;

USTRUCT(Blueprintable, BlueprintType)
struct FRotatorRange
{
	GENERATED_BODY()
	
	FRotatorRange(){}
	FRotatorRange(const FRotator& StartIn, const FRotator& EndIn) : Min(StartIn), Max(EndIn){}
	
	UPROPERTY(Blueprintable, BlueprintReadWrite)
	FRotator Min {FRotator(0.f)};
	UPROPERTY(Blueprintable, BlueprintReadWrite) 
	FRotator Max {FRotator(0.f)};
};

// Represents a joint's state at a given point in time
USTRUCT(Blueprintable, BlueprintType)
struct FUMJointKey
{
	GENERATED_BODY()

public:

	constexpr bool operator<(const FUMJointKey& Other) const
	{
		return Time < Other.Time;
	}
	UPROPERTY(Blueprintable, BlueprintReadWrite)
	float Time = 0.f;
	
	UPROPERTY(Blueprintable, BlueprintReadWrite)
	FTransform Transform = {};

};


// To let the JointKey array be serialized
USTRUCT(Blueprintable, BlueprintType)
struct FUMJointTimeline
{
	GENERATED_BODY()
public:
	FUMJointTimeline() {}
	FUMJointTimeline(const TArray<FUMJointKey>& JointTimelineIn) : Timeline(JointTimelineIn)
	{
		this->Duration = Timeline[Timeline.Num() - 1].Time;
	}
	UPROPERTY(Blueprintable, BlueprintReadWrite)
	TArray<FUMJointKey> Timeline;
	UPROPERTY(BlueprintReadWrite)
	float StartTime = 0;
	UPROPERTY(BlueprintReadWrite)
	float Duration = 0;
public:
	FString ToString();
};

// Represents a single state of a "ctrl" in the control rig for the given joint
USTRUCT(Blueprintable, BlueprintType)
struct FUMJointControl
{
	GENERATED_BODY()
	UPROPERTY(Blueprintable, BlueprintReadWrite)
	FName CtrlName;
	UPROPERTY(Blueprintable, BlueprintReadWrite)
	FRotator CtrlRotator{FRotator(0, 0, 0)};
};

// Represents a bone within the pose-able skeleton
// Holds its own timeline of keys, its own name, and the limit that joint can be rotated within
USTRUCT(Blueprintable, BlueprintType)
struct FUMJoint
{
	GENERATED_BODY()
	FUMJoint() : Name("*") {};
	FUMJoint(FName InName) : Name(InName) {};
	FUMJoint(FName InName, FName InCtrl, FRotatorRange InRangeLimits, FUMJointTimeline InTimeline) :
		Name(InName), CtrlName(InCtrl), RangeLimits(InRangeLimits), Timeline(InTimeline)
	{};
public:
	UPROPERTY(Blueprintable, BlueprintReadWrite)
	FName Name;
	UPROPERTY(Blueprintable, BlueprintReadWrite)
	FName CtrlName;
	UPROPERTY(Blueprintable, BlueprintReadWrite)
	FRotatorRange RangeLimits {FRotatorRange()};
	UPROPERTY(Blueprintable, BlueprintReadWrite)
	FUMJointTimeline Timeline; // Collections of keyframes that have been set for the joint
public:
	FString ToString();
};

// A container for multiple Joints, which can also hold other JointGroups
// Also holds a flattened list of all the Timelines represented within
// Note: underlying Timeline maps being edited does not propagate upward yet
USTRUCT(Blueprintable, BlueprintType)
struct FUMJointGroup
{
	GENERATED_BODY()
	
public:
	UPROPERTY(Blueprintable, BlueprintReadWrite)
	FName Name;
	TArray<FUMJointGroup*> Groups;
	TArray<FUMJoint> Joints;
public:
	FUMJointGroup() : Name("*") {}
	FUMJointGroup(FName InName) : Name(InName) {}
	void AddGroups(TArray<FUMJointGroup*>& GroupsIn) { for (const auto& Group : GroupsIn) { AddGroup(Group); }}
	void AddGroup(FUMJointGroup* Group)
	{
		Groups.Add(Group);
	}
	void AddJoints(TArray<FUMJoint> JointsIn) { for (auto& Joint : JointsIn) { AddJoint(Joint); } }
	void AddJoint(const FUMJoint& Joint)
	{
		Joints.Add(Joint);
	}

	FString ToString();

private:
	
	friend UUMSequenceHelper;
};

UCLASS()
class UUMSequenceHelper :  public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
	UFUNCTION(BlueprintPure, Category = "Animation|Sequencer", meta=(BlueprintThreadSafe))
	static FUMJointKey MakeKeyframe(float Time, const FTransform& Transform);
	
	UFUNCTION(BlueprintPure, Category = "Animation|Sequencer", meta=(BlueprintThreadSafe))
	static FUMJoint MakeJoint(FName NameIn, FName CtrlNameIm, const FRotatorRange& RangeLimitsIn, const FUMJointTimeline& SequenceIn);

	UFUNCTION(BlueprintPure, Category = "Animation|Sequencer", meta=(BlueprintThreadSafe))
	static const FUMJointGroup& MakeJointGroup(FName Name, TArray<FUMJointGroup>& Groups,
	                                           TArray<FUMJoint>& Joints);

	UFUNCTION(BlueprintCallable, Category = "Animation|Joint|Group")
	static const void PrintJointGroup(FUMJointGroup Group);
	
	UFUNCTION(BlueprintPure, Category = "Animation|Joint|Group", meta=(BlueprintThreadSafe))
	static void AddGroups(FUMJointGroup& JointGroup, const TArray<FUMJointGroup>& GroupsIn)
	{
		JointGroup.Groups.Reserve(JointGroup.Groups.Num() + GroupsIn.Num());
		for (auto& Group : GroupsIn) { AddGroup(JointGroup, Group); }
	}
	UFUNCTION(BlueprintPure, Category = "Animation|Joint|Group", meta=(BlueprintThreadSafe))
	static void AddGroup(FUMJointGroup& JointGroup, FUMJointGroup Group)
	{
		// No reserve tricks because they screw up amortized constant time
		JointGroup.Groups.Add(&Group);
	}
	UFUNCTION(BlueprintPure, Category = "Animation|Joint|Group", meta=(BlueprintThreadSafe))
	static void AddJoints(FUMJointGroup& JointGroup, TArray<FUMJoint>& JointsIn)
	{
		JointGroup.Joints.Reserve(JointGroup.Groups.Num() + JointsIn.Num());
		for (auto& Joint : JointsIn) { AddJoint(JointGroup, Joint); }
	}
	UFUNCTION(BlueprintPure, Category = "Animation|Joint|Group", meta=(BlueprintThreadSafe))
	static void AddJoint(FUMJointGroup& JointGroup, const FUMJoint& Joint)
	{
		JointGroup.Joints.Add(Joint);
	}
	UFUNCTION(BlueprintPure, Category = "Animation", meta=(BlueprintThreadSafe))
	static UAnimSequence* BuildSequence(FUMJointGroup JointGroup, USkeletalMesh* SkeletalMesh);
};
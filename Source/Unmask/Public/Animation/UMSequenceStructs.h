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
	FUMJointTimeline(const TArray<FUMJointKey>& JointTimelineIn) : Timeline(JointTimelineIn) {}
	UPROPERTY(Blueprintable, BlueprintReadWrite)
	TArray<FUMJointKey> Timeline;
};


// Represents a bone within the pose-able skeleton
// Holds its own timeline of keys, its own name, and the limit that joint can be rotated within
USTRUCT(Blueprintable, BlueprintType)
struct FUMJoint
{
	GENERATED_BODY()
	FName Name;
	UPROPERTY(Blueprintable, BlueprintReadWrite)
	FRotatorRange RangeLimits {FRotatorRange()};
	UPROPERTY(Blueprintable, BlueprintReadWrite)
	FUMJointTimeline Timeline; // Collections of keyframes that have been set for the joint
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
	static TArray<FUMJointGroup*> AllGroups;
public:
	FUMJointGroup() {
		Me = AllGroups.Num();
		AllGroups.Add(this);
	}
	TArray<FUMJointGroup>& GetGroups() { return Groups; }
	TArray<FUMJoint>& GetJoints() { return Joints; }
	TMap<FName, FUMJointTimeline>& GetAllTimelines() { return AllTimelines; }

	void AddGroups(const TArray<FUMJointGroup>& GroupsIn) { for (auto& Group : GroupsIn) { AddGroup(Group); }}
	void AddGroup(const FUMJointGroup& Group)
	{
		// No reserve tricks because they screw up amortized constant time
		for (const auto& [OldName, Timeline] : Group.AllTimelines)
		{
			AllTimelines.Add(
				FName(Group.Name.ToString() + TEXT(".") + OldName.ToString()), //LowerArm.Hand.Pointer
				Timeline
			);
		}
		Groups.Add(Group);
		Groups.Last().Parent = Me;
	}
	void AddJoints(const TArray<FUMJoint>& JointsIn) { for (auto& Joint : JointsIn) { AddJoint(Joint); } }
	void AddJoint(const FUMJoint& Joint)
	{
		AllTimelines.Add(Joint.Name, Joint.Timeline);
		Joints.Add(Joint);
	}

private:
	int Parent = -1;
	int Me = -1;
	
	TArray<FUMJointGroup> Groups;
	TArray<FUMJoint> Joints;
	TMap<FName, FUMJointTimeline> AllTimelines;

	friend UUMSequenceHelper;
};

UCLASS()
class UUMSequenceHelper :  public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
	UFUNCTION(BlueprintPure, Category = "Animation|Sequencer", meta=(BlueprintThreadSafe))
	static FUMJointKey MakeKeyframe(float Time, const FTransform& Transform);

	UFUNCTION(BlueprintPure, Category = "Animation|Sequencer", meta=(BlueprintThreadSafe))
	static FUMJoint MakeJoint(FName NameIn, const FRotatorRange& RangeLimitsIn, const FUMJointTimeline& SequenceIn);

	UFUNCTION(BlueprintPure, Category = "Animation|Sequencer", meta=(BlueprintThreadSafe))
	static const FUMJointGroup& MakeJointGroup(FName Name, const TArray<FUMJointGroup>& Groups,
	                                           const TArray<FUMJoint>& Joints);

	UFUNCTION(BlueprintPure, Category = "Animation|Joint|Group", meta=(BlueprintThreadSafe))
	static void AddGroups(FUMJointGroup& JointGroup, const TArray<FUMJointGroup>& GroupsIn)
	{
		for (auto& Group : GroupsIn) { AddGroup(JointGroup, Group); }
	}
	UFUNCTION(BlueprintPure, Category = "Animation|Joint|Group", meta=(BlueprintThreadSafe))
	static void AddGroup(FUMJointGroup& JointGroup, const FUMJointGroup& Group)
	{
		// No reserve tricks because they screw up amortized constant time
		for (const auto& [OldName, Timeline] : Group.AllTimelines)
		{
			JointGroup.AllTimelines.Add(
				FName(Group.Name.ToString() + TEXT(".") + OldName.ToString()), 
				Timeline
			);
		}
		JointGroup.Groups.Add(Group);
		//Groups.Last().Parent = Me;
	}
	UFUNCTION(BlueprintPure, Category = "Animation|Joint|Group", meta=(BlueprintThreadSafe))
	static void AddJoints(FUMJointGroup& JointGroup, const TArray<FUMJoint>& JointsIn)
	{
		for (auto& Joint : JointsIn) { AddJoint(JointGroup, Joint); }
	}
	UFUNCTION(BlueprintPure, Category = "Animation|Joint|Group", meta=(BlueprintThreadSafe))
	static void AddJoint(FUMJointGroup& JointGroup, const FUMJoint& Joint)
	{
		JointGroup.AllTimelines.Add(Joint.Name, Joint.Timeline);
		JointGroup.Joints.Add(Joint);
	}
};
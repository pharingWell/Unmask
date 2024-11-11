#pragma once

#include "UMSequenceStructs.generated.h"

USTRUCT(Blueprintable, BlueprintType)
struct FRotatorRange
{
	GENERATED_BODY()
	
	FRotatorRange();
	FRotatorRange(const FRotator& StartIn, const FRotator& EndIn) : Min(StartIn), Max(EndIn){}
	
	UPROPERTY(Blueprintable, BlueprintReadWrite)
	FRotator Min {FRotator()};
	UPROPERTY(Blueprintable, BlueprintReadWrite) 
	FRotator Max {FRotator()};
};

// Represents a joint's state at a given point in time
USTRUCT(Blueprintable, BlueprintType)
struct FUMJointKey
{
	GENERATED_BODY()

public:
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
	TArray<FUMJointKey> Timeline;
};


// Represents a bone within the pose-able skeleton
// Holds its own timeline of keys, its own name, and the limit that joint can be rotated within
USTRUCT(Blueprintable, BlueprintType)
struct FUMJoint
{
	GENERATED_BODY()
	FName Name;
	FRotatorRange RangeLimits {FRotatorRange()};
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
	
	FName Name;
	//static TArray<FUMJointGroup*> AllGroups;
public:
	FUMJointGroup()
	{
// I'd love to add the AllGroups system with the Parent and Me indecies. However...
		
		// unresolved external symbol "public: static class TArray<struct FUMJointGroup *,
		// class TSizedDefaultAllocator<32> > FUMJointGroup::AllGroups"
		// (?AllGroups@FUMJointGroup@@2V?$TArray@PEAUFUMJointGroup@@V?$TSizedDefaultAllocator@$0CA@@@@@A)

		
		// if (AllGroups.Num() == 0)
		// {
		// 	AllGroups = TArray<FUMJointGroup*>();
		// }
		// Me = AllGroups.Num();
		// AllGroups.Add(this);
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
		//Groups.Last().Parent = Me;
	}
	void AddJoints(const TArray<FUMJoint>& JointsIn) { for (auto& Joint : JointsIn) { AddJoint(Joint); } }
	void AddJoint(const FUMJoint& Joint)
	{
		AllTimelines.Add(Joint.Name, Joint.Timeline);
		Joints.Add(Joint);
	}

private:
	// Index in AllGroups array
	//int Parent = -1;
	//int Me = -1;
	
	TArray<FUMJointGroup> Groups;
	TArray<FUMJoint> Joints;
	TMap<FName, FUMJointTimeline> AllTimelines;
};

UCLASS()
class UMSequenceHelper :  public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
	UFUNCTION(BlueprintPure, Category = "Animation|Sequencer", meta=(BlueprintThreadSafe))
	static FUMJointKey MakeKeyframe(float Time, const FTransform& Transform);

	UFUNCTION(BlueprintPure, Category = "Animation|Sequencer", meta=(BlueprintThreadSafe))
	static FUMJoint MakeJoint(FName NameIn, const FRotatorRange& RangeLimitsIn, const FUMJointTimeline& SequenceIn);

	UFUNCTION(BlueprintPure, Category = "Animation|Sequencer", meta=(BlueprintThreadSafe))
	static const FUMJointGroup& MakeJointGroup(FName Name, const TArray<FUMJointGroup>& Groups,
	                                           const TArray<FUMJoint>& Joints);
};
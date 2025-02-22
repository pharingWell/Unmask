// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/UMAnimationRecorder.h"
#include "Animation/Skeleton.h"
#include "Animation/AnimData/UMSequenceStructs.h"

class UUMSequenceHelper;

UUMAnimationRecorder::UUMAnimationRecorder()
{
	this->SkeletalMeshComponent = nullptr;
	RootGroup = NewObject<UUMJointGroup>();
	RootGroup->Name = "Root";
}

UUMAnimationRecorder* UUMAnimationRecorder::GetNewAnimationRecorder()
{
	return NewObject<UUMAnimationRecorder>();
}

void UUMAnimationRecorder::InitAnimationRecorder(USkeletalMeshComponent* InSkeletalMeshComponent)
{
	this->SkeletalMeshComponent = InSkeletalMeshComponent;

	auto MatInsts = SkeletalMeshComponent->GetMaterials();
	
	bool a = true;
	for(auto MatInst : MatInsts)
	{
		DynMatInsts.Add(UMaterialInstanceDynamic::Create(MatInst, this));
	}
	for(int i = 0; i < DynMatInsts.Num(); i++)
	{
		SkeletalMeshComponent->SetMaterial(i, DynMatInsts[i]);
	}


	
	this->RootGroup->Name = FName("body");
	RootGroup->AddBones({"root", "pelvis", "spine_01", "spine_02", "spine_03", "spine_04", "spine_05", "clavicle_l",
		"clavicle_r", });
	this->AllGroups.Add(this->RootGroup->Name, this->RootGroup);
	FUMJoint
		
		LeftShoulder(FName("upperarm_l")),
		RightShoulder(FName("upperarm_r")),
		Pelvis(FName("pelvis")),
		Spine01(FName("spine_01")),
		Spine02(FName("spine_02")),
		Spine03(FName("spine_03")),
		LeftClavicle(FName("clavicle_l")),
		RightClavicle(FName("clavicle_r"));
	RootGroup->AddJoints({LeftShoulder, RightShoulder, Pelvis, Spine01, Spine02, Spine03, LeftClavicle, RightClavicle});

	UUMJointGroup *Head = NewObject<UUMJointGroup>();
	Head->Name = FName("head");
	RootGroup->AddBones({"neck_01", "neck_02", "head"});
	this->AllGroups.Add(Head->Name, Head);
	FUMJoint
		Neck(FName("neck_01")),
		HeadJoint(FName("head"));
	Head->AddJoints({Neck, HeadJoint});
	RootGroup->AddGroup(Head);
	
	UUMJointGroup *LeftArm  = NewObject<UUMJointGroup>();
	LeftArm->Name = FName("arm_l");
	AllGroups.Add(LeftArm->Name, LeftArm);
	LeftArm->AddBones({"thumb_03_l", "hand_l", "index_metacarpal_l", "index_01_l", "index_02_l", "index_03_l",
		"middle_metacarpal_l", "middle_01_l", "middle_02_l", "middle_03_l", "pinky_metacarpal_l", "pinky_01_l",
		"pinky_02_l", "pinky_03_l", "ring_metacarpal_l", "ring_01_l", "ring_02_l", "ring_03_l", "thumb_01_l",
		"thumb_02_l", "lowerarm_l", "lowerarm_twist_01_l", "lowerarm_twist_02_l", "upperarm_l"
	});
	FUMJoint
		LowerLeftArm(FName("lowerarm_l")),
		LeftHand(FName("hand_l")),
		LeftThumb(FName("thumb_01_l")),
		LeftIndex(FName("index_01_l")),
		LeftMiddle(FName("middle_01_l")),
		LeftRing(FName("ring_01_l")),
		LeftPinky(FName("pinky_01_l"));
	LeftArm->AddJoints({LowerLeftArm, LeftHand, LeftThumb, LeftIndex, LeftMiddle, LeftRing, LeftPinky});
	RootGroup->AddGroup(LeftArm);
	
	UUMJointGroup *RightArm = NewObject<UUMJointGroup>();
	RightArm->Name = FName("arm_r");
	AllGroups.Add(RightArm->Name, RightArm);
	RightArm->AddBones({"upperarm_r", "thumb_03_r", "hand_r", "index_metacarpal_r", "index_01_r", "index_02_r", "index_03_r",
		"middle_metacarpal_r", "middle_01_r", "middle_02_r", "middle_03_r", "pinky_metacarpal_r", "pinky_01_r",
		"pinky_02_r", "pinky_03_r", "ring_metacarpal_r", "ring_01_r", "ring_02_r", "ring_03_r", "thumb_01_r",
		"thumb_02_r", "lowerarm_r", "lowerarm_twist_01_r", "lowerarm_twist_02_r"});
	FUMJoint
		LowerRightArm(FName("lowerarm_r")),
		RightHand(FName("hand_r")),
		RightThumb(FName("thumb_01_r")),
		RightIndex(FName("index_01_r")),
		RightMiddle(FName("middle_01_r")),
		RightRing(FName("ring_01_r")),
		RightPinky(FName("pinky_01_r"));
	RightArm->AddJoints({LowerRightArm, RightHand, RightThumb, RightIndex, RightMiddle, RightRing, RightPinky});
	RootGroup->AddGroup(RightArm);

	
	UUMJointGroup *LeftLeg = NewObject<UUMJointGroup>();
	LeftLeg->Name = FName("leg_l");
	AllGroups.Add(LeftLeg->Name, LeftLeg);
	LeftLeg->AddBones({"thigh_l", "calf_l", "calf_twist_01_l", "calf_twist_02_l", "thigh_twist_01_l",
		"thigh_twist_02_l", "foot_l", "ball_l"});
	FUMJoint
		LeftThigh(FName("thigh_l")),
		LeftCalf(FName("calf_l"));
	LeftLeg->AddJoints({LeftThigh, LeftCalf});
	RootGroup->AddGroup(LeftLeg);

	UUMJointGroup *RightLeg = NewObject<UUMJointGroup>();
	RightLeg->Name =  FName("leg_r");
	AllGroups.Add(RightLeg->Name, RightLeg);
	RightLeg->AddBones({"thigh_r","calf_r", "calf_twist_01_r", "calf_twist_02_r", "thigh_twist_01_r", "thigh_twist_02_r", "foot_r", "ball_r"});
	FUMJoint
		RightThigh(FName("thigh_r")),
		RightCalf(FName("calf_r"));
	RightLeg->AddJoints({RightThigh, RightCalf});
	RootGroup->AddGroup(RightLeg);
}

UUMJointGroup* UUMAnimationRecorder::GetGroupWithBone(FName BoneName)
{
	for (auto Tuple : AllGroups)
	{
		auto &Group = Tuple.Value;
		if(Group->Bones.Contains(BoneName))
		{
			UE_LOG(LogCore, Warning, TEXT("%s"), *Group->Name.ToString());
			return Group;
		}
	}
	UE_LOG(LogScript, Warning, TEXT("Failed to find group with Bone %s"), *BoneName.ToString());
	return nullptr;
}

void UUMAnimationRecorder::SelectGroup(UUMJointGroup* Group)
{
	TMap<FName, EUMAnimEditor_MatSlot> MatSlotMap = {
		{"head", HEAD},
		{"body", TORSO},
		{"arm_l", ARM_L},
		{"leg_l", LEG_L},
		{"arm_r", ARM_R},
		{"leg_r", LEG_R},
	};

	TArray<int> MatSlots;
	TArray<FName> ShownGroups;
	ShownGroups.Add(Group->Name);
	for(auto Child : Group->Groups)
	{
		ShownGroups.Add(Child->Name);
	}
	if(Group->Parent)
	{
		ShownGroups.Add(Group->Parent->Name);
	} else
	{
		UE_LOG(LogScript, Error, TEXT("Invalid parent for %s"), *Group->Name.ToString())
	}
	
	
	for (FName Name : ShownGroups)
	{
		UE_LOG(LogCore, Display, TEXT("Name: %s"), *Name.ToString())
		MatSlots.Add(MatSlotMap.FindChecked(Name));
	}
	
	for (int i = 0; i < DynMatInsts.Num(); i++)
	{
		if(MatSlots.Contains(i))
		{
			DynMatInsts[i]->SetScalarParameterValue("bVisible", 1.0);
			continue;
		}
		DynMatInsts[i]->SetScalarParameterValue("bVisible", 0.0);
	}
	
}

FString UUMAnimationRecorder::AllGroupsToString()
{
	auto StringBuilder = new TStringBuilder<1024 * sizeof(FString)>();
	StringBuilder->Append(TEXT("["));
	for (auto Tuple : AllGroups)
	{
		auto& Name = Tuple.Key;
		auto& Group = Tuple.Value;
		StringBuilder->Append(Name.ToString() + TEXT(": \n\tJoints: [") );
		for (int i = 0; i < Group->Joints.Num(); i++)
		{
			auto& Joint = Group->Joints[i];
			StringBuilder->Append(*Joint.Name.ToString());
			if (i < Group->Joints.Num() - 1)
			{
				StringBuilder->Append(TEXT(", "));
			}
		}
		StringBuilder->Append(TEXT("]\n\tGroups: ["));
		for (int i = 0; i < Group->Groups.Num(); i++)
		{
			auto& ChildGroup = Group->Groups[i];
			if(ChildGroup->Name.IsValid())
			{
				StringBuilder->Append(ChildGroup->Name.ToString());
			} else
			{
				StringBuilder->Append("*");
			}
			
			if (i < Group->Groups.Num() - 1)
			{
				StringBuilder->Append(TEXT(", "));
			}
		}
		StringBuilder->Append(TEXT("]\n"));
	}
	return StringBuilder->ToString();
}

void UUMAnimationRecorder::PrintAllGroups()
{
	FString out = AllGroupsToString();
	UE_LOG(LogCore, Display, TEXT("\n%s"), *out);
	if(GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::White,  FString::Printf(TEXT("%s"), *out));
}

void UUMAnimationRecorder::LoadTimelines(TMap<FName, FUMJointTimeline> Timelines)
{
	for (TTuple<FName, FUMJointTimeline> Timeline : Timelines)
	{
		for (TTuple<FName, UUMJointGroup*> Tuple : this->AllGroups)
		{
			bool Exit = false;
			for (int i = 0; i < Tuple.Value->Joints.Num(); i++)
			{
				if (Tuple.Value->Joints[i].Name == Timeline.Key)
				{
					Exit = true;
					(*this->AllGroups.Find(Tuple.Key))->Joints[i].Timeline = Timeline.Value;
					break;
				}
			}
			if (Exit) break;
		}
	}
}

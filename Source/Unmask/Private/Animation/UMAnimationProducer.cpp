// Fill out your copyright notice in the Description page of Project Settings.

#include "Animation/UMAnimationProducer.h"
#include "Animation/UMAnimationConsumer.h"
#include "Animation/AnimationSettings.h"
#include "Animation/BuiltInAttributeTypes.h"
#include "AssetRegistry/AssetRegistryModule.h"
//#include "Developer/AssetTools/Private/AssetTools.h"
#include "ComponentReregisterContext.h"
#include "Animation/AnimData/UMAnimDataController.h"
#include "Animation/AnimData/UMAnimDataModel.h"
#include "Misc/AutomationTest.h"
#include "UObject/SavePackage.h"

static int Counter = 0;

#define FPS 60

IMPLEMENT_SIMPLE_AUTOMATION_TEST(Test_AnimationDetection, "Unmask.Tests.AnimationSystem.Producer",
								 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool Test_AnimationDetection::RunTest(const FString& Parameters)
{
	//AddError(TEXT("Montage is invalid"));
	return true;
}

UAnimSequence* UUMAnimationProducer::CreateSequence(const TMap<FName, FUMJointSequence>& JointTracks,
												   USkeletalMesh* AnimatedObject)
{
	FString SequenceName = "PosedSequence";
	// See if this sequence already exists.
	FString ParentPath = FString::Printf(TEXT("%s/%s"), *FPackageName::GetLongPackagePath(*AnimatedObject->GetOutermost()->GetName()), *SequenceName);
	
	//Import a new sequence
	UPackage *ParentPackage = LoadPackage(nullptr, *ParentPath, LOAD_Verify | LOAD_NoWarn);
	if (ParentPackage != nullptr)
			{
				ParentPackage->FullyLoad();
			} else {
	ParentPackage = CreatePackage(*ParentPath);
	}
	UObject* Object = LoadObject<UObject>(ParentPackage, *SequenceName, NULL, LOAD_None, NULL);
	UAnimSequence *DestSeq = Cast<UAnimSequence>(Object);
	
	// If not, create new one now.
	if (!DestSeq)
	{
	DestSeq = NewObject<UAnimSequence>(ParentPackage, *SequenceName, RF_Public | RF_Standalone);
	// Notify the asset registry
	FAssetRegistryModule::AssetCreated(DestSeq);
	}
	DestSeq->SetSkeleton(AnimatedObject->GetSkeleton());
	IAnimationDataController& Controller = DestSeq->GetController(); // LETS GOOO (Sets to custom DataController)
	Controller.RemoveAllBoneTracks(false);

	// set frame rate
	int32 ResampleRate = 24;
	int32 SequenceLength = 1;
	for (auto Joint : JointTracks)
	{
		SequenceLength = FGenericPlatformMath::Max<int32>(SequenceLength, Joint.Value.JointSequence.Last().Time);
	}
	const FFrameRate ResampleFrameRate(ResampleRate, 1);
	Controller.SetFrameRate(ResampleFrameRate, false);
	const FFrameNumber NumberOfFrames = ResampleFrameRate.AsFrameNumber(SequenceLength);
	Controller.SetNumberOfFrames(FGenericPlatformMath::Max<int32>(NumberOfFrames.Value, 1), false);

	// import blend shape curves
	//
	// int32 CurveAttributeKeyCount = 0;
	// ImportBlendShapeCurves(AnimImportSettings, CurAnimStack, CurveAttributeKeyCount, bReimport);
	for (auto Joint : JointTracks)
	{
		FName &BoneName = Joint.Key;
		TArray<FUMKeyFrame> &JointSequence = Joint.Value.JointSequence;
		const FAnimationCurveIdentifier CurveId(FName(BoneName.ToString() + TEXT("_Curve")), ERawCurveTrackTypes::RCT_Transform);
		Controller.AddCurve(CurveId, EAnimAssetCurveFlags::AACF_NONE, false);
		UUMAnimDataController& UUMController = dynamic_cast<UUMAnimDataController&>(Controller);
		UUMController.SetTransformCurveKeys(CurveId, JointSequence);
		// Reregister skeletal mesh components so they reflect the updated animation
		for (TObjectIterator<USkeletalMeshComponent> Iter; Iter; ++Iter)
		{
	  		FComponentReregisterContext ReregisterContext(*Iter);
		}
	}
	return DestSeq;
}

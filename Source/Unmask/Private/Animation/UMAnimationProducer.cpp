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
#include "Animation/AnimData/UMAnimSequence.h"
#include "Misc/AutomationTest.h"
#include "UObject/SavePackage.h"

static int Counter = 0;

IMPLEMENT_SIMPLE_AUTOMATION_TEST(Test_AnimationDetection, "Unmask.Tests.AnimationSystem.Producer",
								 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool Test_AnimationDetection::RunTest(const FString& Parameters)
{
	//AddError(TEXT("Montage is invalid"));
	return true;
}

UAnimSequence* UUMAnimationProducer::CreateSequence(const TMap<FName, FUMJointTimeline>& JointTimeline,
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
	UObject* Object = LoadObject<UObject>(ParentPackage, *SequenceName, NULL, LOAD_NoWarn, NULL);
	UUMAnimSequence *DestSeq = Cast<UUMAnimSequence>(Object);
	
	// If not, create new one now.
	if (!DestSeq)
	{
		DestSeq = NewObject<UUMAnimSequence>(ParentPackage, *SequenceName, RF_Public | RF_Standalone);
		// Notify the asset registry
		FAssetRegistryModule::AssetCreated(DestSeq);
	}
	DestSeq->SetSkeleton(AnimatedObject->GetSkeleton());
	UUMAnimDataController& UUMController = dynamic_cast<UUMAnimDataController&>(DestSeq->GetUMController()); // LETS GOOO (Sets to custom DataController
	UUMController.RemoveAllBoneTracks(false);
	
	double SequenceLength = 1.0;
	for (auto Joint : JointTimeline)
	{
		SequenceLength = FGenericPlatformMath::Max<int32>(SequenceLength, Joint.Value.Timeline.Last().Time);
	}
	const FFrameRate ResampleFrameRate(ResampleRate, 1);
	UUMController.SetFrameRate(ResampleFrameRate, false);
	const FFrameNumber NumberOfFrames = ResampleFrameRate.AsFrameNumber(SequenceLength);
	UUMController.SetNumberOfFrames(FGenericPlatformMath::Max<int32>(NumberOfFrames.Value, 1), false);

	// import blend shape curves
	//
	// int32 CurveAttributeKeyCount = 0;
	// ImportBlendShapeCurves(AnimImportSettings, CurAnimStack, CurveAttributeKeyCount, bReimport);
	
	for (auto Joint : JointTimeline)
	{
		FName &BoneName = Joint.Key;
		TArray<FUMJointKey> &Timeline = Joint.Value.Timeline;
		for (auto &Keyframe : Timeline)
		{
			Keyframe.Transform.NormalizeRotation();
		}
		FName TrackName = FName(BoneName.ToString() + TEXT("_Curve"));
		const FAnimationCurveIdentifier CurveId(BoneName, ERawCurveTrackTypes::RCT_Transform);
		UUMController.AddCurve(CurveId, EAnimAssetCurveFlags::AACF_NONE, false);
		UUMController.AddBoneCurve(BoneName, false);
		UUMController.SetTransformCurveKeys(CurveId, Timeline);
		FFrameTime FrameTime = FFrameTime(1 + ResampleRate * 3);
		FTransform TransformAt0 = UUMController.GetModel()->EvaluateBoneTrackTransform(TrackName, FrameTime, EAnimInterpolationType::Linear);
		UE_LOG(LogScript, Warning, TEXT("Transform evaluated for BoneTrack %s: [%s]"), *TrackName.ToString(), *TransformAt0.ToString())
	}
	
	// Reregister skeletal mesh components so they reflect the updated animation
	for (TObjectIterator<USkeletalMeshComponent> Iter; Iter; ++Iter)
	{
		FComponentReregisterContext ReregisterContext(*Iter);
	}
	return DestSeq;
}

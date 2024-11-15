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

DEFINE_LOG_CATEGORY(LogAnimProducer);

IMPLEMENT_SIMPLE_AUTOMATION_TEST(Test_AnimationDetection, "Unmask.Tests.AnimationSystem.Producer",
								 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool Test_AnimationDetection::RunTest(const FString& Parameters)
{
	//AddError(TEXT("Montage is invalid"));
	return true;
}

UAnimSequence* UUMAnimationProducer::CreateSequence(TMap<FName, FUMJointTimeline> JointTimelineMap,
												   USkeletalMesh* AnimatedObject)
{
	
	FString SequenceName = FString("PosedSequence") + FString::FromInt(++AnimationNumber);
	UE_LOG(LogAnimProducer, Log, TEXT("Animation: %s"), *SequenceName)
	// See if this sequence already exists.
	FString ParentPath = FString::Printf(TEXT("%s/%s"), *FPackageName::GetLongPackagePath(*AnimatedObject->GetPackage()->GetName()), *SequenceName);
	//Import a new sequence
	//UPackage * ParentPackage = LoadPackage(nullptr, *ParentPath, LOAD_Verify | LOAD_NoWarn);
	UPackage *ParentPackage = CreatePackage(*ParentPath);
	ParentPackage->FullyLoad();
	// if (ParentPackage != nullptr) {
	// 	ParentPackage->FullyLoad();
	// } else {
	// 	
	// }
	//UObject* Object = LoadObject<UObject>(ParentPackage, *SequenceName, NULL, LOAD_NoWarn, NULL);
	//UUMAnimSequence *DestSeq = Cast<UUMAnimSequence>(Object);
	//if(DestSeq == nullptr)
	//{
	// If not, create new one now.
	UUMAnimSequence *DestSeq = NewObject<UUMAnimSequence>(ParentPackage, *SequenceName, RF_Public | RF_Standalone | RF_Transient);
	// Notify the asset registry
	FAssetRegistryModule::AssetCreated(DestSeq);
	//}
	DestSeq->SetSkeleton(AnimatedObject->GetSkeleton());
	UUMAnimDataController& UUMController = dynamic_cast<UUMAnimDataController&>(DestSeq->GetUMController()); // LETS GOOO (Sets to custom DataController
	UUMController.RemoveAllBoneTracks(false);
	
	double SequenceLength = 1.0;
	for (auto Joint : JointTimelineMap)
	{
		TArray<FUMJointKey> &Timeline = Joint.Value.Timeline;
	    if(Timeline.Num() > 0)
	    {
			SequenceLength = FGenericPlatformMath::Max<int32>(SequenceLength, Timeline.Last().Time);
		}
	}
	SequenceLength = FGenericPlatformMath::CeilToDouble(SequenceLength);
	const FFrameRate ResampleFrameRate(ResampleRate, 1);
	UUMController.SetFrameRate(ResampleFrameRate, false);
	const FFrameNumber NumberOfFrames = ResampleFrameRate.AsFrameNumber(SequenceLength);
	UUMController.SetNumberOfFrames(FGenericPlatformMath::Max<int32>(NumberOfFrames.Value, 1), false);
	UUMController.NotifyPopulated();
	for (auto &Joint : JointTimelineMap)
	{
		FName &BoneName = Joint.Key;
		FName TrackName =  BoneName; //FName(BoneName.ToString() + TEXT("_Curve"));
		const FAnimationCurveIdentifier CurveId(TrackName, ERawCurveTrackTypes::RCT_Transform);
		UUMController.AddCurve(CurveId, EAnimAssetCurveFlags::AACF_NONE, false);
		UUMController.AddBoneCurve(TrackName, false);
		//UUMController.SetBoneTrackKeys(BoneName, Joint.Value.Timeline);
		// UE_LOG(LogAnimProducer, Log, TEXT("Joint: %s, Len=%d"), *BoneName.ToString(), Joint.Value.Timeline.Num())
		UUMController.NotifyPopulated();
		UE_LOG(LogAnimProducer, Log, TEXT("Bone: %s"), *TrackName.ToString())
		UUMController.SetTransformCurveKeys(CurveId, Joint.Value.Timeline);
		const FTransformCurve &CurveTransforms = UUMController.Model->GetTransformCurve(CurveId);
		for (int frame = 0; frame < NumberOfFrames.Value; frame++)
		{
			const FTransform &CurveTransform = CurveTransforms.Evaluate(frame/static_cast<double>(ResampleRate), 1.f);
			UE_LOG(LogAnimProducer, Log, TEXT("Transform Frame #%02d: %s"), frame, *CurveTransform.ToString())
		}
	}
	
	// Reregister skeletal mesh components so they reflect the updated animation
	for (TObjectIterator<USkeletalMeshComponent> Iter; Iter; ++Iter)
	{
	 	FComponentReregisterContext ReregisterContext(*Iter);
	}
	return DestSeq;
}

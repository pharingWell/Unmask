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

	// for it to show up in the editor, it needs TWO packages
	
	FString SUBFOLDER = FString("UM_MVP/"); 
	FString PACKAGE_NAME = FString("SequencePackage");
	FString PackageName = FString("/Game/") + SUBFOLDER + PACKAGE_NAME;
	FString SequenceName = FString("PosedSequence") + FString::FromInt(++AnimationNumber);
	UPackage *OuterPackage = CreatePackage(*PackageName);
	FString const PackageFileName = FPackageName::LongPackageNameToFilename(PackageName, FPackageName::GetAssetPackageExtension());
	FString ParentPath = FString::Printf(TEXT("%s/%s"), *FPackageName::GetLongPackagePath(FString(OuterPackage->GetName())), *SequenceName);
	UPackage *ParentPackage = CreatePackage(*ParentPath);
	ParentPackage->FullyLoad();
	UUMAnimSequence *DestSeq = NewObject<UUMAnimSequence>(ParentPackage, *SequenceName, RF_Public | RF_Standalone);
	FAssetRegistryModule::AssetCreated(DestSeq);
	DestSeq->SetSkeleton(AnimatedObject->GetSkeleton());
	
	//IAnimationDataController& UUMController = DestSeq->GetController();
	//UUMController.InitializeModel();

	
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
		TArray<FUMJointKey> &Timeline = Joint.Value.Timeline;
		if(Timeline.Num() <= 0)
		{
			continue;
		}
		Timeline.Sort();
		FName TrackName =  BoneName; 
		UUMController.AddBoneCurve(TrackName, false);
		UUMController.NotifyPopulated();
		FReferenceSkeleton RefSkeleton = AnimatedObject->GetSkeleton()->GetReferenceSkeleton();
		const int Index = RefSkeleton.FindBoneIndex(BoneName);
		const FTransform& RefTransform = RefSkeleton.GetRefBonePose()[Index];
		FTransformCurve CurveTransforms = FTransformCurve();
		for(auto [Time, Transform] : Joint.Value.Timeline) //structured binding
		{
			Transform.AddToTranslation(RefTransform.GetTranslation());
			CurveTransforms.UpdateOrAddKey(Transform, Time);
		}
		TArray<FVector> TranslationOut;
		TArray<FQuat> RotationOut;
		TArray<FVector> ScaleOut;
		UE_LOG(LogAnimProducer, Log, TEXT("Bone: %s"), *TrackName.ToString())
		for (int frame = 0; frame < NumberOfFrames.Value; frame++)
		{
			const FTransform &Transform = CurveTransforms.Evaluate(frame/static_cast<double>(ResampleRate), 1.f);
			UE_LOG(LogAnimProducer, Log, TEXT("Transform Frame #%02d: %s"), frame, *Transform.ToString())
			TranslationOut.Add(Transform.GetTranslation());
			RotationOut.Add(Transform.GetRotation());
			ScaleOut.Add(Transform.GetScale3D());
		}
		UUMController.SetBoneTrackKeys(BoneName, TranslationOut, RotationOut, ScaleOut, false);
	}
	// Reregister skeletal mesh components so they reflect the updated animation
	for (TObjectIterator<USkeletalMeshComponent> Iter; Iter; ++Iter)
	{
	 	FComponentReregisterContext ReregisterContext(*Iter);
	}
	return DestSeq;
}

// Fill out your copyright notice in the Description page of Project Settings.

#include "Animation/UMAnimationProducer.h"
#include "Animation/UMAnimationConsumer.h"
#include "Animation/AnimationSettings.h"
#include "Animation/BuiltInAttributeTypes.h"
#include "AssetRegistry/AssetRegistryModule.h"
//#include "Developer/AssetTools/Private/AssetTools.h"
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
	FMontageBlendSettings BlendInSettings(0.0f);
	FMontageBlendSettings BlendOutSettings(0.0f);
	return CreateSequence_WithBlendSettings(JointTracks, AnimatedObject, BlendInSettings, BlendOutSettings, 0.0f);
}

UAnimSequence* UUMAnimationProducer::CreateSequence(const TMap<FName, FUMJointSequence>& JointTracks,
                                                   USkeletalMesh* AnimatedObject, float BlendInTime, float BlendOutTime,
                                                   float BlendOutTriggerTime)
{
	FMontageBlendSettings BlendInSettings(BlendInTime);
	FMontageBlendSettings BlendOutSettings(BlendOutTime);
	return CreateSequence_WithBlendSettings(JointTracks, AnimatedObject, BlendInSettings, BlendOutSettings, BlendOutTriggerTime);
}

// Code from https://forums.unrealengine.com/t/creating-a-uanimsequence-from-scratch-using-addkeytosequence/774856

UAnimSequence* UUMAnimationProducer::CreateSequence_WithBlendSettings(TMap<FName, FUMJointSequence> JointTracks,
                                                                     USkeletalMesh* AnimatedObject,
                                                                     const FMontageBlendSettings& BlendInSettings,
                                                                     const FMontageBlendSettings& BlendOutSettings,
                                                                     float InBlendOutTriggerTime)
{

	// From https://forums.unrealengine.com/t/create-new-asset-from-code-save-uobject-to-asset/328445/4?u=sf2979
	// and https://georgy.dev/posts/save-uobject-to-package/
	// and https://dev.epicgames.com/community/learning/knowledge-base/wzdm/unreal-engine-how-to-create-new-assets-in-c

	FString SUBFOLDER = FString("UM_MVP/");
	FString PACKAGE_NAME = FString("SequencePackage");
	FString PackageName = FString("/Game/") + SUBFOLDER + PACKAGE_NAME;
	UE_LOG(LogScript, Warning, TEXT("Name: %s"), *PackageName);
	UPackage *OuterPackage = CreatePackage(*PackageName);
	FSavePackageArgs PackageArgs = {};
	{
		PackageArgs.TopLevelFlags = RF_Public | RF_Standalone;
		PackageArgs.bSlowTask = true;
	}

	FString AnimationName = TEXT("Anim_") + FString::FromInt(Counter++);
	FString const PackageFileName = FPackageName::LongPackageNameToFilename(PackageName, FPackageName::GetAssetPackageExtension());
	FString ParentPath = FString::Printf(TEXT("%s/%s"), *FPackageName::GetLongPackagePath(FString(OuterPackage->GetName())), *AnimationName);
	UPackage* ParentPackage = CreatePackage( *ParentPath);
	UAnimSequence* AnimSequence = NewObject<UAnimSequence>(ParentPackage, *AnimationName, RF_Public | RF_Standalone);
	AnimSequence->SetPreviewMesh(AnimatedObject);
	USkeleton* Skeleton = AnimatedObject->GetSkeleton();
	FReferenceSkeleton RefSkeleton = Skeleton->GetReferenceSkeleton();
	AnimSequence->SetSkeleton(Skeleton);
	UE_LOG(LogAnimation, Error, TEXT("1"));
	AnimSequence->Modify();
	FAssetRegistryModule::AssetCreated(AnimSequence);
	//FAnimationCurveData 
	// Setting the framerate has to be done inside the controller.  You can't just set the variables.
	// I don't know why, I just know that this is how you have to do it. Code comes from here:
	// https://forums.unrealengine.com/t/is-there-any-way-to-modify-bone-tracks-with-c/500162/6
	//UAnimationSequencerDataModel
	UE_LOG(LogAnimation, Error, TEXT("2"));
	#if WITH_EDITOR
	#define LOCTEXT_NAMESPACE "UpdateFrameRate"
	float PlayLength = 0.f;
	for (TTuple<FName, FUMJointSequence> Joint : JointTracks)
	{
		if(Joint.Value.JointSequence.Num() > 0)
		{
			PlayLength = FMath::Max( PlayLength, Joint.Value.JointSequence.Last().Time); //the time of the last keyframe in the JointTracks's JointSequence
		}
	}
	// Initialize data model
	// https://docs.unrealengine.com/5.0/en-US/API/Developer/AnimationDataController/UAnimDataController/
	IAnimationDataController& Controller = AnimSequence->GetController();
	Controller.OpenBracket(LOCTEXT("InitializeAnimation", "Initialize New Anim Sequence"));
	{
		Controller.InitializeModel();
		auto DefaultFrameRate = UAnimationSettings::Get()->GetDefaultFrameRate();
		// This line is to set actual frame rate
		Controller.SetNumberOfFrames(FFrameNumber(StaticCast<int32, float>(FPS * PlayLength)), false);
		Controller.SetFrameRate(FFrameRate(FPS, 1), false);  //FFrameRate(numerator, denominator)
		Controller.NotifyPopulated();
	}
	Controller.CloseBracket();

	UE_LOG(LogAnimation, Error, TEXT("3"));
	for (TTuple<FName, FUMJointSequence> Joint : JointTracks)
	{

		FName& BoneName = Joint.Key;
		TArray<FUMKeyFrame>& TrackData = Joint.Value.JointSequence;

		FRawAnimSequenceTrack RawTrack;
		RawTrack.PosKeys.Empty();
		RawTrack.RotKeys.Empty();
		RawTrack.ScaleKeys.Empty();

		//int32 NumKeysForTrack = TrackData.Num();
		TArray<float> TimeKeys;
		TArray<FTransform> TransformValues;
		for(auto [Time, Transform] : TrackData) //structured binding
		{
			//TimeKeys.Add(Time);
			//TransformValues.Add(Transform);



			if (BoneName.IsEqual("Bone_001")) //band-aid
			{
				RawTrack.PosKeys.Add(FVector3f(Transform.GetTranslation())  + FVector3f(0.675f, 0.f, 0.f));
			} else {
				RawTrack.PosKeys.Add(FVector3f(Transform.GetTranslation()));
			}
			RawTrack.RotKeys.Add(FQuat4f(Transform.GetRotation()));
			RawTrack.ScaleKeys.Add( FVector3f(Transform.GetScale3D()));

		}
		// FAnimationCurveIdentifier Id;
		// {
		// 	Id.CurveType = ERawCurveTrackTypes::RCT_Transform;
		// 	Id.CurveName = FName(AnimationName);
		// }
		// Controller.AddCurve(Id);
		// Controller.SetTransformCurveKeys(Id, TransformValues, TimeKeys, false);
		Controller.AddBoneCurve(BoneName);
		Controller.SetBoneTrackKeys(BoneName, RawTrack.PosKeys, RawTrack.RotKeys, RawTrack.ScaleKeys, false);
		// UE::Anim::AddTypedCustomAttribute<FTransformAnimationAttribute, FTransform>(FName(BoneName), BoneName,
		//  AnimSequence, MakeArrayView(TimeKeys.GetData(), NumKeysForTrack),
		// MakeArrayView(TransformValues.GetData(), NumKeysForTrack), false);
	}
	#endif
	#undef LOCTEXT
	//https://forums.unrealengine.com/t/is-there-any-way-to-modify-bone-tracks-with-c/500162/6?u=sf2979
	return AnimSequence;
}

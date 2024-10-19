// Fill out your copyright notice in the Description page of Project Settings.

#include "Animation/UMAnimationProducer.h"

#include "AssetViewUtils.h"
#include "Animation/AnimationSettings.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "GenericPlatform/GenericPlatformOutputDevices.h"
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


UAnimMontage* UUMAnimationProducer::CreateMontage(const TMap<FName, FUMJointSequence>& JointTracks, USkeletalMesh* AnimatedObject)
{
	FMontageBlendSettings BlendInSettings(0.0f);
	FMontageBlendSettings BlendOutSettings(0.0f);
	return CreateMontage_WithBlendSettings(JointTracks, AnimatedObject, BlendInSettings, BlendOutSettings, 0.0f);
}

UAnimMontage* UUMAnimationProducer::CreateMontage(const TMap<FName, FUMJointSequence>& JointTracks, USkeletalMesh* AnimatedObject, float BlendInTime, float BlendOutTime, float BlendOutTriggerTime)
{
	FMontageBlendSettings BlendInSettings(BlendInTime);
	FMontageBlendSettings BlendOutSettings(BlendOutTime);
	return CreateMontage_WithBlendSettings(JointTracks, AnimatedObject, BlendInSettings, BlendOutSettings, BlendOutTriggerTime);
}

UAnimMontage* UUMAnimationProducer::CreateMontage_WithBlendSettings(TMap<FName, FUMJointSequence> JointTracks, USkeletalMesh* AnimatedObject, const FMontageBlendSettings& BlendInSettings, const FMontageBlendSettings& BlendOutSettings, float InBlendOutTriggerTime)
{
	// From https://forums.unrealengine.com/t/create-new-asset-from-code-save-uobject-to-asset/328445/4?u=sf2979
	// and https://georgy.dev/posts/save-uobject-to-package/
	FString PACKAGE_NAME = FString("Montage1");
	FString SUBFOLDER = FString("UM_MVP/");
	FString PackagePath = FString("/Game/") + SUBFOLDER + PACKAGE_NAME;
	UPackage *Package = CreatePackage(*PackagePath);
	FSavePackageArgs PackageArgs = {};
	{
		PackageArgs.TopLevelFlags = RF_Public | RF_Standalone;
		PackageArgs.SaveFlags = SAVE_None;
		PackageArgs.Error = FGenericPlatformOutputDevices::GetLog();
		PackageArgs.bSlowTask = true;
	}
	UAnimMontage* NewMontage = NewObject<UAnimMontage>(Package, UAnimMontage::StaticClass(),
		FName(FString("AM_") + PACKAGE_NAME), EObjectFlags::RF_Standalone);
	NewMontage->SetSkeleton(AnimatedObject->GetSkeleton());
	
	FAssetRegistryModule::AssetCreated(NewMontage);
	bool bDirty = NewMontage->MarkPackageDirty();
	if (bDirty)
	{
		UE_LOG(LogScript, Warning, TEXT("Ye not be dirty!"))
	}
	// #if WITH_EDITOR
	// #define LOCTEXT_NAMESPACE "Set Segment Length"
	// IAnimationDataController& Controller = NewMontage->GetController();
	// #endif
	int TrackIndex = 0;
	for (TTuple<FName, FUMJointSequence> Joint : JointTracks)
	{
		FName& JointName = Joint.Key;
		TArray<FUMKeyFrame>& TrackData = Joint.Value.JointSequence;
		FName AnimationName = FName(JointName.ToString() + TEXT("_") + FString::FromInt(Counter++));
		UAnimSequence* AnimSequence = NewObject<UAnimSequence>(
			Package,
			UAnimSequence::StaticClass(),
			AnimationName,
			EObjectFlags::RF_Standalone
		);
		// Notify asset registry
		FAssetRegistryModule::AssetCreated(AnimSequence);
		bDirty = AnimSequence->MarkPackageDirty();
		if (bDirty)
		{
			UE_LOG(LogScript, Warning, TEXT("Ye not be dirty!"))
		}
		// Set skeleton (you need to do this before you add animations to it or it will throw an error)
		AnimSequence->ResetAnimation();
		AnimSequence->SetSkeleton(AnimatedObject->GetSkeleton());
		
		// Setting the framerate has to be done inside the controller.  You can't just set the variables.  
		// I don't know why, I just know that this is how you have to do it. Code comes from here:
		// https://forums.unrealengine.com/t/is-there-any-way-to-modify-bone-tracks-with-c/500162/6
		
		#if WITH_EDITOR
		#define LOCTEXT_NAMESPACE "UpdateFrameRate"
		float PlayLength = 0.f;
		if(TrackData.Num() > 0)
		{
			PlayLength = TrackData.Last().Time; //the time of the first keyframe in the JointTracks's JointSequence
		}
		// Initialize data model
		// https://docs.unrealengine.com/5.0/en-US/API/Developer/AnimationDataController/UAnimDataController/
		IAnimationDataController& Controller = AnimSequence->GetController();
		Controller.OpenBracket(LOCTEXT("InitializeAnimation", "Initialize New Anim Sequence"));
		{
			Controller.InitializeModel();
			auto DefaultFrameRate = UAnimationSettings::Get()->GetDefaultFrameRate();
			// This line is to set actual frame rate
			Controller.SetNumberOfFrames(FFrameNumber(StaticCast<int32, float>(DefaultFrameRate.AsDecimal() * PlayLength)), true);
			Controller.SetFrameRate(FFrameRate(FPS, 1), true);  //FFrameRate(numerator, denominator)
			Controller.NotifyPopulated();
		}

		#endif
		#undef LOCTEXT
		
		for(FUMKeyFrame Keyframe : TrackData)
		{
			AnimSequence->AddKeyToSequence(Keyframe.Time, JointName, Keyframe.Transform);
			//AnimSequence->GetController().AddBoneCurve(JointName);
		}
		AnimSequence->GetController().NotifyPopulated();
		bool bSuccess = UPackage::SavePackage(Package, Cast<UObject,UAnimSequence>(*AnimSequence), *Package->GetLoadedPath().GetLocalFullPath(), PackageArgs);
		if (!bSuccess)
		{
			UE_LOG(LogSavePackage, Error, TEXT("Failed to save package. %s"), *Package->GetName());
		}
		
		FSlotAnimationTrack Blank = {};
		FName SlotName = FName(AnimationName.ToString() + "_Slot");
		FSlotAnimationTrack& NewTrack = NewMontage->AddSlot(SlotName); // Skip default slot

		// add new track
		NewTrack.SlotName = JointName;
		FAnimSegment NewSegment;
		NewSegment.SetAnimReference(AnimSequence, true);
		NewTrack.AnimTrack.AnimSegments.Add(NewSegment);
		TrackIndex++;
	}
	
	NewMontage->BlendIn = FAlphaBlend(BlendInSettings.Blend);
	NewMontage->BlendModeIn = BlendInSettings.BlendMode;
	NewMontage->BlendProfileIn = BlendInSettings.BlendProfile;
	NewMontage->BlendOut = FAlphaBlend(BlendOutSettings.Blend);
	NewMontage->BlendModeOut = BlendOutSettings.BlendMode;
	NewMontage->BlendProfileOut = BlendOutSettings.BlendProfile;
	NewMontage->GetCurveData();
	NewMontage->BlendOutTriggerTime = InBlendOutTriggerTime;
	
	bool bSuccess = UPackage::SavePackage(Package, Cast<UObject,UAnimMontage>(*NewMontage), *Package->GetLoadedPath().GetLocalFullPath(), PackageArgs);
	if (!bSuccess)
	{
		UE_LOG(LogSavePackage, Error, TEXT("Failed to save package. %s"), *Package->GetName());
	}
	return NewMontage;
}
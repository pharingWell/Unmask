// Fill out your copyright notice in the Description page of Project Settings.

#include "Animation/UMAnimationProducer.h"

#include "AssetToolsModule.h"
#include "AssetViewUtils.h"
#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"
#include "Animation/AnimationSettings.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Factories/AnimSequenceFactory.h"
#include "GenericPlatform/GenericPlatformOutputDevices.h"
#include "Misc/AutomationTest.h"
#include "UObject/SavePackage.h"
#if WITH_EDITOR
#include "Factories/AnimSequenceFactory.h"
#endif
static int Counter = 0;

#define FPS 60

IMPLEMENT_SIMPLE_AUTOMATION_TEST(Test_AnimationDetection, "Unmask.Tests.AnimationSystem.Producer",
								 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool Test_AnimationDetection::RunTest(const FString& Parameters)
{
	//AddError(TEXT("Montage is invalid"));
	return true;
}


UAnimSequence* UUMAnimationProducer::CreateMontage(const TMap<FName, FUMJointSequence>& JointTracks,
                                                   USkeletalMesh* AnimatedObject)
{
	FMontageBlendSettings BlendInSettings(0.0f);
	FMontageBlendSettings BlendOutSettings(0.0f);
	return CreateMontage_WithBlendSettings(JointTracks, AnimatedObject, BlendInSettings, BlendOutSettings, 0.0f);
}

UAnimSequence* UUMAnimationProducer::CreateMontage(const TMap<FName, FUMJointSequence>& JointTracks,
                                                   USkeletalMesh* AnimatedObject, float BlendInTime, float BlendOutTime,
                                                   float BlendOutTriggerTime)
{
	FMontageBlendSettings BlendInSettings(BlendInTime);
	FMontageBlendSettings BlendOutSettings(BlendOutTime);
	return CreateMontage_WithBlendSettings(JointTracks, AnimatedObject, BlendInSettings, BlendOutSettings, BlendOutTriggerTime);
}

// Code from https://forums.unrealengine.com/t/creating-a-uanimsequence-from-scratch-using-addkeytosequence/774856

UAnimSequence* UUMAnimationProducer::CreateMontage_WithBlendSettings(TMap<FName, FUMJointSequence> JointTracks,
                                                                     USkeletalMesh* AnimatedObject,
                                                                     const FMontageBlendSettings& BlendInSettings,
                                                                     const FMontageBlendSettings& BlendOutSettings,
                                                                     float InBlendOutTriggerTime)
{
	// From https://forums.unrealengine.com/t/create-new-asset-from-code-save-uobject-to-asset/328445/4?u=sf2979
	// and https://georgy.dev/posts/save-uobject-to-package/
	// and https://dev.epicgames.com/community/learning/knowledge-base/wzdm/unreal-engine-how-to-create-new-assets-in-c

	FString SUBFOLDER = FString("UM_MVP/");
	FString PACKAGE_NAME = FString("Sequence2");
	FString PackageName = FString("/Game/") + SUBFOLDER + PACKAGE_NAME;
	UE_LOG(LogScript, Warning, TEXT("Name: %s"), *PackageName);
	UPackage *Package = CreatePackage(*PackageName);
	FSavePackageArgs PackageArgs = {};
	{
		PackageArgs.TopLevelFlags = RF_Public | RF_Standalone;
		PackageArgs.bSlowTask = true;
	}
	FName AnimationName = FName(TEXT("Anim_") + FString::FromInt(Counter++));
#if 1 == 0
	UAnimSequenceFactory* Factory = NewObject<UAnimSequenceFactory>();
	Factory->PreviewSkeletalMesh = AnimatedObject;
	Factory->TargetSkeleton = AnimatedObject->GetSkeleton();
	UAnimSequence* AnimSequence  = Cast<UAnimSequence>(Factory->FactoryCreateNew(UAnimSequence::StaticClass(),
		Package, AnimationName, EObjectFlags::RF_Standalone, AnimatedObject, nullptr));
	
	// Cast<UAnimSequence>(AssetToolsModule.Get().CreateAsset(Name, PackagePath,
	// UAnimSequence::StaticClass(), Factory, AnimatedObject->GetName(), Errors));
#else
	UAnimSequence* AnimSequence = NewObject<UAnimSequence>(
	 Package,
	 UAnimSequence::StaticClass(),
	 AnimationName,
	 EObjectFlags::RF_Standalone
	 );
#endif
	AnimSequence->SetPreviewMesh(AnimatedObject);
	AnimSequence->SetSkeleton(AnimatedObject->GetSkeleton());
	// Set skeleton (you need to do this before you add animations to it or it will throw an error)
	AnimSequence->ResetAnimation();
	
	// Notify asset registry
	FAssetRegistryModule::AssetCreated(AnimSequence);

	// Setting the framerate has to be done inside the controller.  You can't just set the variables.  
	// I don't know why, I just know that this is how you have to do it. Code comes from here:
	// https://forums.unrealengine.com/t/is-there-any-way-to-modify-bone-tracks-with-c/500162/6
	//UAnimationSequencerDataModel
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
		Controller.SetNumberOfFrames(FFrameNumber(StaticCast<int32, float>(DefaultFrameRate.AsDecimal() * PlayLength)), true);
		Controller.SetFrameRate(FFrameRate(FPS, 1), true);  //FFrameRate(numerator, denominator)
		Controller.NotifyPopulated();
	}
	#endif
	#undef LOCTEXT
	
	for (TTuple<FName, FUMJointSequence> Joint : JointTracks)
	{
		for(TArray<FUMKeyFrame>& TrackData = Joint.Value.JointSequence; auto [Time, Transform] : TrackData) //structured binding
		{
			AnimSequence->AddKeyToSequence(Time, Joint.Key, Transform);
		}
	}

	AnimSequence->GetController().NotifyPopulated();

	FSavePackageResultStruct SuccessResult = UPackage::Save(Package, Cast<UObject,UAnimSequence>(*AnimSequence), *Package->GetLoadedPath().GetLocalFullPath(), PackageArgs);
	if (!SuccessResult.IsSuccessful())
	{
		UE_LOG(LogSavePackage, Error, TEXT("Failed to save package. %s"), *Package->GetName());
		return AnimSequence;
	}
	
	
	

	// Tell content browser to show the newly created asset
	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
	
	TArray<UObject*> Objects;
	Objects.Add(AnimSequence);
	ContentBrowserModule.Get().SyncBrowserToAssets(Objects);
	
	return AnimSequence;
}
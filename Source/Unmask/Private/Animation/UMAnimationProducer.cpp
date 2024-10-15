// Fill out your copyright notice in the Description page of Project Settings.

#include "Animation/UMAnimationProducer.h"
#include "AssetRegistry/AssetRegistryModule.h"

static int Counter = 0;

/*// From https://forums.unrealengine.com/t/creating-a-uanimsequence-from-scratch-using-addkeytosequence/774856/5?u=sf2979
UAnimMontage* UUMAnimationProducer::CreateMontage(
	const TMap<FName, TArray<FUMKeyFrame>>& JointTracks, USkeletalMesh* AnimatedObject
){
	UAnimSequence* AnimSequence = NewObject<UAnimSequence>(
		Cast<UObject, USkeletalMesh>(AnimatedObject),
		UAnimSequence::StaticClass(),
		FName(TEXT("Anim_") + FString::FromInt(Counter++)),
		EObjectFlags::RF_Public
	);

	// Notify asset registry
	FAssetRegistryModule::AssetCreated(AnimSequence);
	
	// Set skeleton (you need to do this before you add animations to it or it will throw an error)
	AnimSequence->ResetAnimation();
	AnimSequence->SetSkeleton(AnimatedObject->GetSkeleton());

	// Setting the framerate has to be done inside the controller.  You can't just set the variables.  
	// I don't know why, I just know that this is how you have to do it. Code comes from here:
	// https://forums.unrealengine.com/t/is-there-any-way-to-modify-bone-tracks-with-c/500162/6

	#if WITH_EDITOR
	#define LOCTEXT_NAMESPACE "UpdateFrameRate"

		// Initialize data model
		// https://docs.unrealengine.com/5.0/en-US/API/Developer/AnimationDataController/UAnimDataController/
	IAnimationDataController& Controller = AnimSequence->GetController();
	Controller.OpenBracket(LOCTEXT("InitializeAnimation", "Initialize New Anim Sequence"));
	{
		// This line is to set actual frame rate
		Controller.SetFrameRate(FFrameRate(FPS, 1), true);  //FFrameRate(numerator, denominator)
		//Controller.SetPlayLength(double(NumberOfFramesInThisAnimationClip / FPS), true);
		Controller.NotifyPopulated();
	}
	Controller.CloseBracket();

	#undef LOCTEXT_NAMESPACE
	#endif

	return CreateMontage(JointTracks, AnimSequence);
}*/

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

	// now play
	UAnimMontage* NewMontage = NewObject<UAnimMontage>();
	NewMontage->SetSkeleton(AnimatedObject->GetSkeleton());
	
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
		Cast<UObject, USkeletalMesh>(AnimatedObject),
		UAnimSequence::StaticClass(),
		AnimationName,
		EObjectFlags::RF_Public
	);

		// Notify asset registry
		FAssetRegistryModule::AssetCreated(AnimSequence);
		
		// Set skeleton (you need to do this before you add animations to it or it will throw an error)
		AnimSequence->ResetAnimation();
		AnimSequence->SetSkeleton(AnimatedObject->GetSkeleton());

		for(FUMKeyFrame Keyframe : TrackData)
		{
			AnimSequence->AddKeyToSequence(Keyframe.Time, JointName, Keyframe.Transform);
			//AnimSequence->GetController().AddBoneCurve(JointName);
		}
		AnimSequence->GetController().NotifyPopulated();
		int32 NewSlot;
		if (TrackIndex == 0)
		{
			NewSlot = 0;
		}else{
			NewSlot = NewMontage->SlotAnimTracks.AddDefaulted(1);
		}
		NewMontage->SlotAnimTracks[NewSlot].SlotName = JointName;

		// add new track
		FSlotAnimationTrack& NewTrack = NewMontage->SlotAnimTracks[NewSlot];
		NewTrack.SlotName = JointName;
		FAnimSegment NewSegment;
		NewSegment.SetAnimReference(AnimSequence, true);

		
		
		//NewSegment.LoopingCount = LoopCount;
		
		/*#if WITH_EDITOR
		// Initialize data model
		// https://docs.unrealengine.com/5.0/en-US/API/Developer/AnimationDataController/UAnimDataController/
		
		Controller.OpenBracket(LOCTEXT("InitializeAnimation", "Initialize New Anim Segment Length"));
		{
			Controller.SetFrameRate(UAnimationSettings::Get()->GetDefaultFrameRate(), true);
			Controller.NotifyPopulated();
		}
		Controller.CloseBracket();

		#undef LOCTEXT_NAMESPACE
		#endif*/
		
		NewTrack.AnimTrack.AnimSegments.Add(NewSegment);

		FCompositeSection NewSection;
		
		NewSection.SectionName = AnimationName;
		NewSection.Link(AnimSequence, AnimSequence->GetPlayLength());
		float StartTime = 0.0f;
		if(TrackData.Num() > 0)
		{
			StartTime = TrackData[0].Time; //the time of the first keyframe in the JointTracks's JointSequence
		}
		NewSection.SetTime(StartTime);

		// add new section
		NewMontage->CompositeSections.Add(NewSection);
		TrackIndex++;
	}

	NewMontage->BlendIn = FAlphaBlend(BlendInSettings.Blend);
	NewMontage->BlendModeIn = BlendInSettings.BlendMode;
	NewMontage->BlendProfileIn = BlendInSettings.BlendProfile;

	NewMontage->BlendOut = FAlphaBlend(BlendOutSettings.Blend);
	NewMontage->BlendModeOut = BlendOutSettings.BlendMode;
	NewMontage->BlendProfileOut = BlendOutSettings.BlendProfile;

	NewMontage->BlendOutTriggerTime = InBlendOutTriggerTime;
	return NewMontage;
}
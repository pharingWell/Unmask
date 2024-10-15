// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UMSequenceStructs.h"
#include "Editor/Experimental/EditorInteractiveToolsFramework/Public/Behaviors/2DViewportBehaviorTargets.h"
#include "UObject/Object.h"
#include "UMAnimationProducer.generated.h"

UCLASS()
class UNMASK_API UUMAnimationProducer : public UObject
{
	GENERATED_BODY()

public:
	static TObjectPtr<UAnimMontage> CreateMontage(const TMap<FName, FUMJointSequence>& JointTracks, USkeletalMesh* AnimatedObject);
	static TObjectPtr<UAnimMontage> CreateMontage(const TMap<FName, FUMJointSequence>& JointTracks, UAnimSequenceBase* Asset);
	static TObjectPtr<UAnimMontage> CreateMontage(const TMap<FName, FUMJointSequence>& JointTracks, UAnimSequenceBase* Asset,
	                                              float BlendInTime, float BlendOutTime, float BlendOutTriggerTime);
	static TObjectPtr<UAnimMontage> CreateMontage_WithBlendSettings(TMap<FName, FUMJointSequence> JointTracks, UAnimSequenceBase* Asset,
	                                                                const FMontageBlendSettings& BlendInSettings, const FMontageBlendSettings& BlendOutSettings, float InBlendOutTriggerTime);

	
};

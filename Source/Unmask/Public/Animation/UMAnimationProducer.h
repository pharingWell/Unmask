// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UMSequenceStructs.h"
#include "UObject/Object.h"
#include "UMAnimationProducer.generated.h"


UCLASS()
class UNMASK_API UUMAnimationProducer : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	static UAnimSequence* CreateMontage(const TMap<FName, FUMJointSequence>& JointTracks, USkeletalMesh* AnimatedObject);
	static UAnimSequence* CreateMontage(const TMap<FName, FUMJointSequence>& JointTracks, USkeletalMesh* AnimatedObject,
	                                    float BlendInTime, float BlendOutTime, float BlendOutTriggerTime);
	static UAnimSequence* CreateMontage_WithBlendSettings(TMap<FName, FUMJointSequence> JointTracks,
	                                                      USkeletalMesh* AnimatedObject,
	                                                      const FMontageBlendSettings& BlendInSettings,
	                                                      const FMontageBlendSettings&
	                                                      BlendOutSettings, float InBlendOutTriggerTime);

	
};

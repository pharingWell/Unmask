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
	static TObjectPtr<UAnimMontage> CreateMontage(USkeletalMesh* AnimatedObject, TMap<FName, FUMJointSequence> BodyJointSequences);
	UAnimMontage* CreateSlotAnimationAsDynamicMontage_WithBlendSettings(UAnimSequenceBase* Asset, FName SlotNodeName,
	                                                                    const FMontageBlendSettings& BlendInSettings,
	                                                                    const FMontageBlendSettings& BlendOutSettings,
	                                                                    float InPlayRate, int32 LoopCount,
	                                                                    float InBlendOutTriggerTime);
};

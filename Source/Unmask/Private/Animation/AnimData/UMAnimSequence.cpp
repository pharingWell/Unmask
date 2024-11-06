// Fill out your copyright notice in the Description page of Project Settings.

#include "Animation/AnimData/UMAnimSequence.h"

#include "AnimationData/Private/AnimSequencerDataModel.h"
#include "Animation/AnimData/UMAnimDataController.h"
#include "Animation/AnimData/UMAnimDataModel.h"


void UUMAnimSequence::CorrectModel()
{
	checkf(Controller, TEXT("Failed to create AnimationDataController"));
	UClass* TargetClass = UUMAnimDataModel::StaticClass();
	UObject* ClassDataModel = NewObject<UObject>(this, TargetClass, TargetClass->GetFName());
	DataModelInterface = ClassDataModel;
	UUMAnimDataController *ControllerObj = NewObject<UUMAnimDataController>(GetTransientPackage());
	Controller = TScriptInterface<UUMAnimDataController>(ControllerObj);
	Controller->SetModel(DataModelInterface);
	ensureAlways(Controller->GetModelInterface() == DataModelInterface);
	BindToModelModificationEvent();
	if (USkeleton* MySkeleton = GetSkeleton()) //PreloadSkeleton
	{
		if (FLinkerLoad* SkeletonLinker = MySkeleton->GetLinker())
		{
			SkeletonLinker->Preload(MySkeleton);
		}
		MySkeleton->ConditionalPostLoad();
	}else
	{
		UE_LOG(LogScript, Error, TEXT("Where me bones???"))
	}
	bModelCorrected = true;
	
}

void UUMAnimSequence::PostLoad()
{
	bModelCorrected = false;
	Super::PostLoad();
	CorrectModel();
	UE_LOG(LogScript, Warning, TEXT("Successfully copied model"));
	checkf(DataModelInterface.GetObject()->GetClass() == UUMAnimDataModel::StaticClass(), TEXT("Invalid DMI class"))
}

IAnimationDataController& UUMAnimSequence::GetUMController()
{
	ValidateModel();
	if(!bModelCorrected)
	{
		CorrectModel();
	}
	return *Controller;
}
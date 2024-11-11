// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimData/UMAnimSequence.h"
#include "Animation/AnimSequenceBase.h"
#include "Animation/AnimData/UMAnimDataController.h"
#include "Animation/AnimData/UMAnimDataModel.h"


void UUMAnimSequence::CorrectModel()
{
	checkf(Controller, TEXT("Failed to create AnimationDataController"));
	UClass* TargetClass = UUMAnimDataModel::StaticClass();
	UObject* ClassDataModel = NewObject<UObject>(this, TargetClass, TargetClass->GetFName()); //Based off of AnimSequenceBase's Create Model
	DataModelInterface = ClassDataModel;
	BindToModelModificationEvent();
	if(!bModelCorrected)
	{
		UUMAnimDataController *ControllerObj = NewObject<UUMAnimDataController>(GetTransientPackage());
		Controller = TScriptInterface<UUMAnimDataController>(ControllerObj);
		Controller->SetModel(DataModelInterface);
		bModelCorrected = true;
	}
	ensureAlways(Controller->GetModelInterface() == DataModelInterface);
	
	if (USkeleton* MySkeleton = GetSkeleton()) //PreloadSkeleton
	{
		if (FLinkerLoad* SkeletonLinker = MySkeleton->GetLinker())
		{
			SkeletonLinker->Preload(MySkeleton);
		}
		MySkeleton->ConditionalPostLoad();
	}
	
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

#if !WITH_EDITOR

/** Callback registered to UAnimDatModel::GetModifiedEvent for the embedded object */
void OnModelModified(const EAnimDataModelNotifyType& NotifyType, IAnimationDataModel* Model, const FAnimDataModelNotifPayload& Payload);

void UUMAnimSequence::BindToModelModificationEvent()
{
	ValidateModel();
	DataModelInterface->GetModifiedEvent().RemoveAll(this);
	DataModelInterface->GetModifiedEvent().AddUObject(this, &UUMAnimSequence::OnModelModified);
}

void UUMAnimSequence::ValidateModel() const
{
	checkf(DataModelInterface != nullptr, TEXT("Invalid AnimSequenceBase state (%s), no data model found"), *GetPathName());
}

void UUMAnimSequence::OnModelModified(const EAnimDataModelNotifyType& NotifyType, IAnimationDataModel* Model, const FAnimDataModelNotifPayload& Payload)
{

	auto HandleNumberOfFramesChange = [&](int32 NewNumFrames, int32 OldNumFrames, int32 Frame0, int32 Frame1)
	{
		const FFrameRate& ModelFrameRate = DataModelInterface->GetFrameRate();
		const float T0 = ModelFrameRate.AsSeconds(Frame0);
		const float T1 = ModelFrameRate.AsSeconds(Frame1);
		const float NewLength = ModelFrameRate.AsSeconds(NewNumFrames);
		
		const float StartRemoveTime = ModelFrameRate.AsSeconds(Frame0);
		const float EndRemoveTime = ModelFrameRate.AsSeconds(Frame1);

		// Total time value for frames that were removed
		const float Duration = T1 - T0;
		
	};

	// Copy any float/transform curves from the model into RawCurveData, as it is used at runtime for AnimMontage/Composite(s)
	auto CopyCurvesFromModel = [this]()
	{
		PRAGMA_DISABLE_DEPRECATION_WARNINGS
		RawCurveData.FloatCurves = DataModelInterface->GetCurveData().FloatCurves;
		//RawCurveData.TransformCurves = DataModelInterface->GetCurveData().TransformCurves;
		PRAGMA_ENABLE_DEPRECATION_WARNINGS
	};

	bool bShouldMarkPackageDirty = !FUObjectThreadContext::Get().IsRoutingPostLoad && NotifyType != EAnimDataModelNotifyType::BracketOpened;
}

#endif
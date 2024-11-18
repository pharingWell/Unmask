// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UMAnimDataController.h"
#include "Animation/AnimSequence.h"
#include "UMAnimSequence.generated.h"

/**
 * 
 */
UCLASS()
class UNMASK_API UUMAnimSequence : public UAnimSequence
{
	GENERATED_BODY()
public:
	void CorrectModel();
	virtual void PostLoad() override;
	IAnimationDataController& GetUMController();

#if !WITH_EDITORONLY_DATA
protected:
	
	/** IAnimationDataModel instance containing (source) animation data */
	TScriptInterface<IAnimationDataModel> DataModelInterface;

	/** Flag set whenever the data-model is initially populated (during upgrade path) */
	bool bPopulatingDataModel;

	/** UAnimDataController instance set to operate on DataModel */
	TScriptInterface<IAnimationDataController> Controller;

#endif // WITH_EDITORONLY_DATA
#if !WITH_EDITOR

	/** Callback registered to UAnimDatModel::GetModifiedEvent for the embedded object */
	ENGINE_API virtual void OnModelModified(const EAnimDataModelNotifyType& NotifyType, IAnimationDataModel* Model, const FAnimDataModelNotifPayload& Payload);
	
	/** Binds to DataModel its modification delegate */
	ENGINE_API void BindToModelModificationEvent();

	/** Validates that DataModel contains a valid UAnimDataModel object */
	ENGINE_API void ValidateModel() const;
	
#endif

private:
	bool bModelCorrected = false;
	bool bControllerCorrected = false;
	bool bFirstCorrection = true;
};

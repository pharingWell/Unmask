// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimData/AnimDataModel.h"
#include "UMAnimDataModel.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, meta=(DebugTreeLeaf))
class UNMASK_API UUMAnimDataModel : public UObject, public IAnimationDataModel
{
	GENERATED_BODY()
public:
	/** Begin UObject overrides */
	UNMASK_API virtual void PostLoad() override;
	UNMASK_API virtual void PostDuplicate(bool bDuplicateForPIE) override;
	virtual bool IsEditorOnly() const override { return true; }
	/** End UObject overrides */
	
	/** Begin IAnimationDataModel overrides */
	UNMASK_API virtual double GetPlayLength() const override;
	UNMASK_API virtual int32 GetNumberOfFrames() const override;
	UNMASK_API virtual int32 GetNumberOfKeys() const override;
	UNMASK_API virtual FFrameRate GetFrameRate() const override;
	
	virtual const TArray<FBoneAnimationTrack>& GetBoneAnimationTracks() const override;
	UNMASK_API virtual const FBoneAnimationTrack& GetBoneTrackByIndex(int32 TrackIndex) const override;
	UNMASK_API virtual const FBoneAnimationTrack& GetBoneTrackByName(FName TrackName) const override;
	UNMASK_API virtual const FBoneAnimationTrack* FindBoneTrackByName(FName Name) const override;
	UNMASK_API virtual const FBoneAnimationTrack* FindBoneTrackByIndex(int32 BoneIndex) const override;
	UNMASK_API virtual int32 GetBoneTrackIndex(const FBoneAnimationTrack& Track) const override;
	UNMASK_API virtual int32 GetBoneTrackIndexByName(FName TrackName) const override;
	UNMASK_API virtual bool IsValidBoneTrackIndex(int32 TrackIndex) const override;
	
	virtual int32 GetNumBoneTracks() const override;
	UNMASK_API virtual void GetBoneTrackNames(TArray<FName>& OutNames) const override;
	UNMASK_API virtual FTransform GetBoneTrackTransform(FName TrackName, const FFrameNumber& FrameNumber) const override;
	UNMASK_API virtual void GetBoneTrackTransforms(FName TrackName, const TArray<FFrameNumber>& FrameNumbers, TArray<FTransform>& OutTransforms) const override;	
	UNMASK_API virtual void GetBoneTrackTransforms(FName TrackName, TArray<FTransform>& OutTransforms) const override;
	UNMASK_API virtual void GetBoneTracksTransform(const TArray<FName>& TrackNames, const FFrameNumber& FrameNumber, TArray<FTransform>& OutTransforms) const override;	
	UNMASK_API virtual FTransform EvaluateBoneTrackTransform(FName TrackName, const FFrameTime& FrameTime, const EAnimInterpolationType& Interpolation) const override;
	UNMASK_API virtual bool IsValidBoneTrackName(const FName& TrackName) const override;	
	UNMASK_API virtual const FAnimCurveBase* FindCurve(const FAnimationCurveIdentifier& CurveIdentifier) const override;
	UNMASK_API virtual const FFloatCurve* FindFloatCurve(const FAnimationCurveIdentifier& CurveIdentifier) const override;
	UNMASK_API virtual const FTransformCurve* FindTransformCurve(const FAnimationCurveIdentifier& CurveIdentifier) const override;
	UNMASK_API virtual const FRichCurve* FindRichCurve(const FAnimationCurveIdentifier& CurveIdentifier) const override;
	UNMASK_API virtual const FAnimationCurveData& GetCurveData() const override;
	UNMASK_API virtual int32 GetNumberOfTransformCurves() const override;
	UNMASK_API virtual int32 GetNumberOfFloatCurves() const override;
	UNMASK_API virtual const TArray<struct FFloatCurve>& GetFloatCurves() const override;
	UNMASK_API virtual const TArray<struct FTransformCurve>& GetTransformCurves() const override;	
	UNMASK_API virtual const FAnimCurveBase& GetCurve(const FAnimationCurveIdentifier& CurveIdentifier) const override;
	UNMASK_API virtual const FFloatCurve& GetFloatCurve(const FAnimationCurveIdentifier& CurveIdentifier) const override;
	UNMASK_API virtual const FTransformCurve& GetTransformCurve(const FAnimationCurveIdentifier& CurveIdentifier) const override;
	UNMASK_API virtual const FRichCurve& GetRichCurve(const FAnimationCurveIdentifier& CurveIdentifier) const override;
	UNMASK_API virtual TArrayView<const FAnimatedBoneAttribute> GetAttributes() const override;
	UNMASK_API virtual int32 GetNumberOfAttributes() const override;
	UNMASK_API virtual int32 GetNumberOfAttributesForBoneIndex(const int32 BoneIndex) const override;
	UNMASK_API virtual void GetAttributesForBone(const FName& BoneName, TArray<const FAnimatedBoneAttribute*>& OutBoneAttributes) const override;
	UNMASK_API virtual const FAnimatedBoneAttribute& GetAttribute(const FAnimationAttributeIdentifier& AttributeIdentifier) const override;
	UNMASK_API virtual const FAnimatedBoneAttribute* FindAttribute(const FAnimationAttributeIdentifier& AttributeIdentifier) const override;
	UNMASK_API virtual UAnimSequence* GetAnimationSequence() const override;
	virtual FAnimDataModelModifiedEvent& GetModifiedEvent() override { return ModifiedEvent; }
	UNMASK_API virtual FGuid GenerateGuid() const override;
#if WITH_EDITOR
	UNMASK_API virtual void Evaluate(FAnimationPoseData& InOutPoseData, const UE::Anim::DataModel::FEvaluationContext& EvaluationContext) const override;
#endif
	UNMASK_API virtual TScriptInterface<IAnimationDataController> GetController() override;
	virtual bool HasBeenPopulated() const override { return bPopulated; }
	UNMASK_API virtual void IterateBoneKeys(const FName& BoneName, TFunction<bool(const FVector3f& Pos, const FQuat4f&, const FVector3f, const FFrameNumber&)> IterationFunction) const override;
protected:
	UNMASK_API virtual IAnimationDataModel::FModelNotifier& GetNotifier() override;
	virtual FAnimDataModelModifiedDynamicEvent& GetModifiedDynamicEvent() override { return ModifiedEventDynamic; }
	virtual void OnNotify(const EAnimDataModelNotifyType& NotifyType, const FAnimDataModelNotifPayload& Payload) override {}
	virtual void LockEvaluationAndModification() const override final {}
	virtual bool TryLockEvaluationAndModification() const override final { return true; }
	virtual void UnlockEvaluationAndModification() const override final {}
	/** End IAnimationDataModel overrides */

private:
	/** Helper functionality used by UAnimDataController to retrieve mutable data */ 
	UNMASK_API FBoneAnimationTrack* FindMutableBoneTrackByName(FName Name);
	UNMASK_API FBoneAnimationTrack& GetMutableBoneTrackByName(FName Name);
	UNMASK_API FTransformCurve* FindMutableTransformCurveById(const FAnimationCurveIdentifier& CurveIdentifier);
	UNMASK_API FFloatCurve* FindMutableFloatCurveById(const FAnimationCurveIdentifier& CurveIdentifier);
	UNMASK_API FAnimCurveBase* FindMutableCurveById(const FAnimationCurveIdentifier& CurveIdentifier);	   
	UNMASK_API FRichCurve* GetMutableRichCurve(const FAnimationCurveIdentifier& CurveIdentifier);

	UNMASK_API USkeleton* GetSkeleton() const;

private:
	UPROPERTY(Transient)
	int32 BracketCounter = 0;

	/** Dynamic delegate event allows scripting to register to any broadcasted notify. */
	UPROPERTY(BlueprintAssignable, Transient, Category = AnimationDataModel, meta = (ScriptName = "ModifiedEvent", AllowPrivateAccess = "true"))
	FAnimDataModelModifiedDynamicEvent ModifiedEventDynamic;
	
	/** Native delegate event allows for registerings to any broadcasted notify. */
	FAnimDataModelModifiedEvent ModifiedEvent;

	/** All individual bone animation tracks */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Animation Data Model", meta = (AllowPrivateAccess = "true"))
	TArray<FBoneAnimationTrack> BoneAnimationTracks;

	/** Total playable length of the contained animation data */
	UE_DEPRECATED(5.1, "PlayLength is deprecated use GetPlayLength instead, as it is now calculated with Number of Frames * FrameRate instead of stored as a value")
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation Data Model", meta = (AllowPrivateAccess = "true"))
	float PlayLength;
	
	/** Rate at which the animated data is sampled */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation Data Model", meta = (AllowPrivateAccess = "true"))
	FFrameRate FrameRate;

	/** Total number of sampled animated frames */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation Data Model", meta = (AllowPrivateAccess = "true"))
	int32 NumberOfFrames;

	/** Total number of sampled animated keys */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation Data Model", meta = (AllowPrivateAccess = "true"))
	int32 NumberOfKeys;
	
	/** Container with all animated curve data */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation Data Model", meta = (AllowPrivateAccess = "true"))
	FAnimationCurveData CurveData;
	
	/** Container with all animated (bone) attribute data */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation Data Model", meta = (AllowPrivateAccess = "true"))
	TArray<FAnimatedBoneAttribute> AnimatedBoneAttributes;

	UPROPERTY()
	bool bPopulated = false;

	TUniquePtr<IAnimationDataModel::FModelNotifier> Notifier;

	friend class UUMAnimDataController;
};

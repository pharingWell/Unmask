// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PoseableMeshComponent.h"
#include "Components/SkinnedMeshComponent.h"
//#include "Interfaces/Interface_CollisionDataProvider.h"



#include "UMPoseableMeshComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UNMASK_API UUMPoseableMeshComponent : public UPoseableMeshComponent, public IInterface_CollisionDataProvider
{
	GENERATED_BODY()

public:
	/*FPhysicsAggregateHandle Aggregate;
	/** Array of FBodyInstance objects, storing per-instance state about about each body. #1#
	TArray<struct FBodyInstance*> Bodies;
	/** Array of FConstraintInstance structs, storing per-instance state about each constraint. #1#
	TArray<struct FConstraintInstance*> Constraints;
	/** 
	 *	Index of the 'Root Body', or top body in the asset hierarchy. 
	 *	Filled in by InitInstance, so we don't need to save it.
	 #1#
	/** To save root body index/bone index consistently *#1#
	struct 
	{
		int32 BodyIndex;
		FTransform TransformToRoot;
	} RootBodyData;*/
	
public:
	// Sets default values for this component's properties
	UUMPoseableMeshComponent();


	/*
	// Called every frame

	USkeletalMesh* GetSkeletalMeshAsset() const;
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	virtual void OnCreatePhysicsState() override;
	void InitArticulated(FPhysScene* PhysScene);
	void InstantiatePhysicsAsset(const UPhysicsAsset& PhysAsset, const FVector& Scale3D, TArray<FBodyInstance*>& OutBodies, TArray<FConstraintInstance*>& OutConstraints, FPhysScene* PhysScene = nullptr, UUMPoseableMeshComponent* OwningComponent = nullptr, int32 UseRootBodyIndex = INDEX_NONE, const FPhysicsAggregateHandle& UseAggregate = FPhysicsAggregateHandle()) const;
	void InstantiatePhysicsAsset_Internal(const UPhysicsAsset& PhysAsset, const FVector& Scale3D, TArray<FBodyInstance*>& OutBodies, TArray<FConstraintInstance*>& OutConstraints, TFunctionRef<FTransform(int32)> BoneTransformGetter, FPhysScene* PhysScene = nullptr, UUMPoseableMeshComponent* OwningComponent = nullptr, int32 UseRootBodyIndex = INDEX_NONE, const FPhysicsAggregateHandle& UseAggregate = FPhysicsAggregateHandle()) const;
	void InitCollisionRelationships();

	/** Find the root body index#1#
	int32 FindRootBodyIndex() const;*/

protected:
		// Called when the game starts
		virtual void BeginPlay() override;

};

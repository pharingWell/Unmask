// Fill out your copyright notice in the Description page of Project Settings.


#include "UMPoseableMeshComponent.h"

//#include "Chaos/PhysicsObjectInterface.h"
//#include "PhysicsEngine/PhysicsAsset.h"

// Sets default values for this component's properties
UUMPoseableMeshComponent::UUMPoseableMeshComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UUMPoseableMeshComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
}
/*
USkeletalMesh* UUMPoseableMeshComponent::GetSkeletalMeshAsset() const
{
	return Cast<USkeletalMesh>(GetSkinnedAsset());
}




// Called every frame
void UUMPoseableMeshComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                             FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UUMPoseableMeshComponent::OnCreatePhysicsState()
{
	InitArticulated(GetWorld()->GetPhysicsScene());
	USceneComponent::OnCreatePhysicsState(); // Need to route CreatePhysicsState, skip PrimitiveComponent
}

void UUMPoseableMeshComponent::InitArticulated(FPhysScene* PhysScene)
{ 
	UPhysicsAsset* const PhysicsAsset = GetPhysicsAsset();

	if (PhysScene == nullptr || PhysicsAsset == nullptr || GetSkeletalMeshAsset() == nullptr || !
		ShouldCreatePhysicsState())
	{
		return;
	}

	if (Bodies.Num() > 0)
	{
		UE_LOG(LogSkeletalMesh, Log,
		       TEXT(
			       "UUMPoseableMeshComponent::InitArticulated : Bodies already created (%s) - call TermArticulated first."
		       ), *GetPathName());
		return;
	}

	// Skip if not desired on dedicated server
	UWorld* World = GetWorld();
	if (PhysicsAsset->bNotForDedicatedServer && World && (World->GetNetMode() == NM_DedicatedServer) &&
		bAllowNotForDedServerPhysicsAssets)
	{
		UE_LOG(LogSkeletalMesh, Verbose, TEXT("Skipping PhysicsAsset creation on dedicated server (%s : %s) %s"),
		       *GetNameSafe(GetOuter()), *GetName(), *PhysicsAsset->GetName());
		return;
	}

	FVector Scale3D = GetComponentTransform().GetScale3D();

	// Find root physics body
	RootBodyData.BodyIndex = INDEX_NONE; //Reset the root body index just in case we need to re-find a new one
	const int32 RootBodyIndex = FindRootBodyIndex();

	if (RootBodyIndex == INDEX_NONE)
	{
		UE_LOG(LogSkeletalMesh, Log,
		       TEXT("UUMPoseableMeshComponent::InitArticulated : Could not find root physics body: '%s'"),
		       *GetPathName());
		return;
	}

	// Set up the map from skelmeshcomp ID to collision disable table
	int32 NumShapes = 0;
	const int32 NumBodies = PhysicsAsset->SkeletalBodySetups.Num();
	for (int32 BodyIndex = 0; BodyIndex < NumBodies; ++BodyIndex)
	{
		if (PhysicsAsset->SkeletalBodySetups[BodyIndex])
		{
			NumShapes += PhysicsAsset->SkeletalBodySetups[BodyIndex]->AggGeom.GetElementCount();
		}
	}

	if (!Aggregate.IsValid() && NumShapes > RagdollAggregateThreshold && NumShapes <= AggregateMaxSize)
	{
		Aggregate = FPhysicsInterface::CreateAggregate(PhysicsAsset->SkeletalBodySetups.Num());
	}
	else if (Aggregate.IsValid() && NumShapes > AggregateMaxSize)
	{
		UE_LOG(LogSkeletalMesh, Log,
		       TEXT("UUMPoseableMeshComponent::InitArticulated : Too many shapes to create aggregate, Max: %u, This: %d"
		       ), AggregateMaxSize, NumShapes);
	}

	UUMPoseableMeshComponent::InstantiatePhysicsAsset(*PhysicsAsset, Scale3D, Bodies, Constraints, PhysScene, this, RootBodyIndex, Aggregate);
	for (int32 BodyIndex = 0; BodyIndex < NumBodies; ++BodyIndex)
	{
		if (FBodyInstance* Body = Bodies[BodyIndex])
		{
			if (!Body->ActorHandle)
			{
				continue;
			}

			Chaos::FPhysicsObject* PhysicsObject = Body->ActorHandle->GetPhysicsObject();
			if (UBodySetup* Setup = Body->GetBodySetup())
			{
				Chaos::FPhysicsObjectInterface::SetName(PhysicsObject, Setup->BoneName);
			}
			Chaos::FPhysicsObjectInterface::SetId(PhysicsObject, BodyIndex);
		}
	}

	// now update root body index because body has BodySetup now
	SetRootBodyIndex(RootBodyIndex);

	InitCollisionRelationships();

	// Update Flag
	PrevRootBoneMatrix = GetBoneMatrix(0); // save the root bone transform

	// pre-compute cloth teleport thresholds for performance
	ComputeTeleportDistanceThresholdInRadians();
	ComputeTeleportRotationThresholdInRadians();
}

void UUMPoseableMeshComponent::InstantiatePhysicsAsset(const UPhysicsAsset& PhysAsset, const FVector& Scale3D, TArray<FBodyInstance*>& OutBodies,
                             TArray<FConstraintInstance*>& OutConstraints, FPhysScene* PhysScene,
                             UUMPoseableMeshComponent* OwningComponent, int32 UseRootBodyIndex,
                             const FPhysicsAggregateHandle& UseAggregate) const
{
	auto BoneTMCallable = [this](int32 BoneIndex)
	{
		return GetBoneTransform(BoneIndex);
	};
	InstantiatePhysicsAsset_Internal(PhysAsset, Scale3D, OutBodies, OutConstraints, BoneTMCallable, PhysScene,
		OwningComponent, UseRootBodyIndex, UseAggregate);
}

void UUMPoseableMeshComponent::InstantiatePhysicsAsset_Internal(const UPhysicsAsset& PhysAsset, const FVector& Scale3D,
	TArray<FBodyInstance*>& OutBodies, TArray<FConstraintInstance*>& OutConstraints,
	TFunctionRef<FTransform(int32)> BoneTransformGetter, FPhysScene* PhysScene, UUMPoseableMeshComponent* OwningComponent,
	int32 UseRootBodyIndex, const FPhysicsAggregateHandle& UseAggregate) const
{
	
}

void UUMPoseableMeshComponent::InitCollisionRelationships()
{
	if (UPhysicsAsset* const PhysicsAsset = GetPhysicsAsset())
	{
		int32 NumDisabledCollisions = PhysicsAsset->CollisionDisableTable.Num();
		if (NumDisabledCollisions)
		{
			TMap<FPhysicsActorHandle, TArray< FPhysicsActorHandle > > DisabledCollisions;
			for (auto& Elem : PhysicsAsset->CollisionDisableTable)
			{		
				// @question : PhysicsAsset->CollisionDisableTable should contain direct indices into the Bodies list?
				//             I saw some OOB errors in a baked build that seemed to indicate that is not the case.

				int32 SourceIndex = Elem.Key.Indices[0];
				int32 TargetIndex = Elem.Key.Indices[1];
				bool bDoCollide = !Elem.Value;
				if (0 <= SourceIndex && SourceIndex < Bodies.Num())
				{
					if (auto* SourceBody = Bodies[SourceIndex])
					{
						if (0 <= TargetIndex && TargetIndex < Bodies.Num())
						{
							if (auto* TargetBody = Bodies[TargetIndex])
							{
								if (FPhysicsActorHandle SourceHandle = SourceBody->GetPhysicsActorHandle())
								{
									if (FPhysicsActorHandle TargetHandle = TargetBody->GetPhysicsActorHandle())
									{
										if (!DisabledCollisions.Contains(SourceHandle))
										{
											DisabledCollisions.Add(SourceHandle, TArray<FPhysicsActorHandle>());
											DisabledCollisions[SourceHandle].Reserve(NumDisabledCollisions);
										}

										checkSlow(!DisabledCollisions[SourceHandle].Contains(TargetHandle));
										DisabledCollisions[SourceHandle].Add(TargetHandle);
									}
								}
							}
						}
					}
				}
			}
			FPhysicsCommand::ExecuteWrite(this, [&]()
				{
					FChaosEngineInterface::AddDisabledCollisionsFor_AssumesLocked(DisabledCollisions);
				});
		}
	}
}

int32 UUMPoseableMeshComponent::FindRootBodyIndex() const
{
	// Find root physics body
	int32 RootBodyIndex = RootBodyData.BodyIndex;
	if(RootBodyIndex == INDEX_NONE && GetSkeletalMeshAsset())
	{
		if(const UPhysicsAsset* PhysicsAsset = GetPhysicsAsset())
		{
			for (int32 i = 0; i< GetSkeletalMeshAsset()->GetRefSkeleton().GetNum(); i++)
			{
				int32 BodyInstIndex = PhysicsAsset->FindBodyIndex(GetSkeletalMeshAsset()->GetRefSkeleton().GetBoneName(i));
				if (BodyInstIndex != INDEX_NONE)
				{
					RootBodyIndex = BodyInstIndex;
					break;
				}
			}
		}
	}

	return RootBodyIndex;
}
*/

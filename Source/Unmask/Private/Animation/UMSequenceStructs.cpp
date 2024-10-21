#include "Animation/UMSequenceStructs.h"

#include "Components/PoseableMeshComponent.h"

FUMKeyFrame UMSequenceHelper::MakeKeyframe(float time, FTransform transform)
{
	return {0.0f, FTransform(UE::Math::TVector(0.0))};
}

FUMJointSequence UMSequenceHelper::MakeJointSequence(const TArray<FUMKeyFrame>& joints)
{
	
	return {joints};
}	
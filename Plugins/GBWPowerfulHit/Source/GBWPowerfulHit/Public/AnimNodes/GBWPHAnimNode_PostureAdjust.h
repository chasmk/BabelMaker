// Copyright GanBowen 2022-2023. All Rights Reserved.

#pragma once

#include "BoneControllers/AnimNode_SkeletalControlBase.h"
#include "GBWPHBPISet.h"
#include "GBWPHAnimNode_PostureAdjust.generated.h"

USTRUCT(BlueprintInternalUseOnly)
struct GBWPOWERFULHIT_API FGBWPHAnimNode_PostureAdjust : public FAnimNode_SkeletalControlBase
{
	GENERATED_USTRUCT_BODY()
	
	FGBWPHAnimNode_PostureAdjust();

	// FAnimNode_Base interface
	virtual void GatherDebugData(FNodeDebugData& DebugData) override;
	// End of FAnimNode_Base interface

	// FAnimNode_SkeletalControlBase interface
	virtual void EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms) override;
	virtual bool IsValidToEvaluate(const USkeleton* Skeleton, const FBoneContainer& RequiredBones) override;
	virtual void UpdateInternal(const FAnimationUpdateContext& Context) override
	{
		DeltaTime = Context.GetDeltaTime();
	};
	// End of FAnimNode_SkeletalControlBase interface

private:
	// FAnimNode_SkeletalControlBase interface
	virtual void InitializeBoneReferences(const FBoneContainer& RequiredBones) override;
	// End of FAnimNode_SkeletalControlBase interface

	bool CheckOwner(FComponentSpacePoseContext Context);
	bool CheckValid(FComponentSpacePoseContext Context);

	float DeltaTime = 0.0f;
	
	AActor* OwnerActor = nullptr;
	UActorComponent* PHAnimInterfaceComponent = nullptr;

	//For PostureAdjust
	bool PostureAdjustActiveCache = false;
	FGBWPH_Anim_PostureAdjuestData PostureAdjuestDataCache = FGBWPH_Anim_PostureAdjuestData();
	FRotator BaseRotationCache = FRotator::ZeroRotator;
	FRotator TargetRotationCache = FRotator::ZeroRotator;
	float interpSpeedCache = 0.0f;
	float interpSpeedMax = 10.0f;
	float interpSpeedSpeed = 4.0f;
	
	//For BodyShake
	float BodyShakeTimer = 0.0f;
	//For BodyTwist
	float BodyTwistTimer = 0.0f;
};

// Copyright GanBowen 2022-2023. All Rights Reserved.

#include "AnimNodes/GBWPHAnimNode_PostureAdjust.h"
#include "Animation/AnimInstanceProxy.h"
#include "Kismet/KismetMathLibrary.h"

FGBWPHAnimNode_PostureAdjust::FGBWPHAnimNode_PostureAdjust()
{
}

void FGBWPHAnimNode_PostureAdjust::GatherDebugData(FNodeDebugData& DebugData)
{
	
}

void FGBWPHAnimNode_PostureAdjust::EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms)
{
	if (!CheckValid(Output))
	{
		return;
	}

	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(EvaluateSkeletalControl_AnyThread)
	const FBoneContainer& BoneContainer = Output.Pose.GetPose().GetBoneContainer();
	FTransform ComponentTransform = Output.AnimInstanceProxy->GetComponentTransform();

	TMap<FCompactPoseBoneIndex, FTransform> OutRes;
	
//For PostureAdjust Start
	{
		bool PostureAdjustActive = false;
		FGBWPH_Anim_PostureAdjuestData PostureAdjuestData = FGBWPH_Anim_PostureAdjuestData();

		if (PHAnimInterfaceComponent)
		{
			PostureAdjustActive = IGBWPHBPI_Anim::Execute_GetPostureAdjustData(PHAnimInterfaceComponent,PostureAdjuestData);
		}
		else
		{
			PostureAdjustActive = false;
		}

		if (PostureAdjustActive != PostureAdjustActiveCache)
		{
			interpSpeedCache = 0.0f;
		}
		PostureAdjustActiveCache = PostureAdjustActive;
		
		if (PostureAdjustActive)
		{
			PostureAdjuestDataCache = PostureAdjuestData;
			
			FBoneReference PostureRootBone = FBoneReference(PostureAdjuestData.PostureRootBoneName);
			if (PostureRootBone.Initialize(BoneContainer))
			{
				FCompactPoseBoneIndex CompactPoseBoneToModify = PostureRootBone.GetCompactPoseIndex(BoneContainer);
				FTransform NewBoneTM = Output.Pose.GetComponentSpaceTransform(CompactPoseBoneToModify);
				if (OutRes.Find(CompactPoseBoneToModify))
				{
					NewBoneTM = OutRes.FindRef(CompactPoseBoneToModify);
				}

				interpSpeedCache = UKismetMathLibrary::FInterpTo(
					interpSpeedCache,
					interpSpeedMax,
					DeltaTime,
					interpSpeedSpeed);
				
				BaseRotationCache = UKismetMathLibrary::RInterpTo(BaseRotationCache,
					FRotator(0.0f,PostureAdjuestData.BaseYawAdjust,PostureAdjuestData.BaseRollAdjust),
					DeltaTime,
					interpSpeedCache);
				const FQuat BaseBoneQuat(BaseRotationCache);
				NewBoneTM.SetRotation(BaseBoneQuat * NewBoneTM.GetRotation());

				FRotator TargetRotation = UKismetMathLibrary::FindRelativeLookAtRotation(Output.AnimInstanceProxy->GetActorTransform(),PostureAdjuestData.TargetPoint);

				FRotator Rotation = FRotator(0.0f,TargetRotation.Yaw,TargetRotation.Pitch*-1.0f);

				Rotation.Yaw = UKismetMathLibrary::FClamp(Rotation.Yaw,PostureAdjuestData.YawAdjustRange.X,PostureAdjuestData.YawAdjustRange.Y);
				Rotation.Roll = UKismetMathLibrary::FClamp(Rotation.Roll,PostureAdjuestData.RollAdjustRange.X ,PostureAdjuestData.RollAdjustRange.Y);

				TargetRotationCache = UKismetMathLibrary::RInterpTo(
					TargetRotationCache,
					Rotation,
					DeltaTime,
					interpSpeedCache);

				if (UKismetMathLibrary::EqualEqual_RotatorRotator(TargetRotationCache,Rotation,0.1f))
				{
					interpSpeedCache = 0.0f;
				}
				
				// Convert to Bone Space.
				FAnimationRuntime::ConvertCSTransformToBoneSpace(ComponentTransform, Output.Pose, NewBoneTM, CompactPoseBoneToModify, EBoneControlSpace::BCS_ComponentSpace);	
				const FQuat BoneQuat(TargetRotationCache);
				NewBoneTM.SetRotation(BoneQuat * NewBoneTM.GetRotation());

				// Convert back to Component Space.
				FAnimationRuntime::ConvertBoneSpaceTransformToCS(ComponentTransform, Output.Pose, NewBoneTM, CompactPoseBoneToModify, EBoneControlSpace::BCS_ComponentSpace);
	
				//OutBoneTransforms.Add( FBoneTransform(PostureRootBone.GetCompactPoseIndex(BoneContainer), NewBoneTM) );
				OutRes.Add(CompactPoseBoneToModify,NewBoneTM);
			}
		}
		else if (BaseRotationCache != FRotator::ZeroRotator || TargetRotationCache != FRotator::ZeroRotator)
		{
			FBoneReference PostureRootBone = FBoneReference(PostureAdjuestDataCache.PostureRootBoneName);
			if (PostureRootBone.Initialize(BoneContainer))
			{
				FCompactPoseBoneIndex CompactPoseBoneToModify = PostureRootBone.GetCompactPoseIndex(BoneContainer);
				FTransform NewBoneTM = Output.Pose.GetComponentSpaceTransform(CompactPoseBoneToModify);
				if (OutRes.Find(CompactPoseBoneToModify))
				{
					NewBoneTM = OutRes.FindRef(CompactPoseBoneToModify);
				}

				interpSpeedCache = UKismetMathLibrary::FInterpTo(
					interpSpeedCache,
					interpSpeedMax,
					DeltaTime,
					interpSpeedSpeed/2);

				BaseRotationCache = UKismetMathLibrary::RInterpTo(BaseRotationCache,
					FRotator::ZeroRotator,
					DeltaTime,
					interpSpeedCache);
				const FQuat BaseBoneQuat(BaseRotationCache);
				NewBoneTM.SetRotation(BaseBoneQuat * NewBoneTM.GetRotation());

				FRotator Rotation = FRotator::ZeroRotator;

				TargetRotationCache = UKismetMathLibrary::RInterpTo(TargetRotationCache,
								Rotation,
								DeltaTime,
								interpSpeedCache);

				if (UKismetMathLibrary::EqualEqual_RotatorRotator(TargetRotationCache,Rotation,0.1f))
				{
					interpSpeedCache = 0.0f;
					BaseRotationCache = FRotator::ZeroRotator;
					TargetRotationCache = FRotator::ZeroRotator;
				}
				
				// Convert to Bone Space.
				FAnimationRuntime::ConvertCSTransformToBoneSpace(ComponentTransform, Output.Pose, NewBoneTM, CompactPoseBoneToModify, EBoneControlSpace::BCS_ComponentSpace);	
				const FQuat BoneQuat(TargetRotationCache);
				NewBoneTM.SetRotation(BoneQuat * NewBoneTM.GetRotation());

				// Convert back to Component Space.
				FAnimationRuntime::ConvertBoneSpaceTransformToCS(ComponentTransform, Output.Pose, NewBoneTM, CompactPoseBoneToModify, EBoneControlSpace::BCS_ComponentSpace);
	
				//OutBoneTransforms.Add( FBoneTransform(PostureRootBone.GetCompactPoseIndex(BoneContainer), NewBoneTM) );
				OutRes.Add(CompactPoseBoneToModify,NewBoneTM);
			}
		}
	}
//For PostureAdjust End

//For BodyShake Start
	{
		bool BodyShakeActive = false;
		FGBWPH_Anim_BodyShakeData BodyShakeData = FGBWPH_Anim_BodyShakeData();
		
		BodyShakeActive = IGBWPHBPI_Anim::Execute_GetBodyShakeData(PHAnimInterfaceComponent,false,BodyShakeData);
		
		if (BodyShakeActive)
		{
			if (BodyShakeTimer>BodyShakeData.Duration)
			{
				IGBWPHBPI_Anim::Execute_GetBodyShakeData(PHAnimInterfaceComponent,true,BodyShakeData);
				BodyShakeTimer = 0.0f;
			}
			else
			{
				BodyShakeTimer+=DeltaTime;
				
				FBoneReference BodyShakeBone = FBoneReference(BodyShakeData.BoneName);
				if (BodyShakeBone.Initialize(BoneContainer))
				{
					FCompactPoseBoneIndex CompactPoseBoneToModify = BodyShakeBone.GetCompactPoseIndex(BoneContainer);
					FTransform NewBoneTM = Output.Pose.GetComponentSpaceTransform(CompactPoseBoneToModify);
					if (OutRes.Find(CompactPoseBoneToModify))
					{
						NewBoneTM = OutRes.FindRef(CompactPoseBoneToModify);
					}

					FVector Translation = FVector(
						UKismetMathLibrary::RandomFloatInRange(-BodyShakeData.Degree,BodyShakeData.Degree),
						UKismetMathLibrary::RandomFloatInRange(-BodyShakeData.Degree,BodyShakeData.Degree),
						UKismetMathLibrary::RandomFloatInRange(-BodyShakeData.Degree,BodyShakeData.Degree));
		
					// Convert to Bone Space.
					FAnimationRuntime::ConvertCSTransformToBoneSpace(ComponentTransform, Output.Pose, NewBoneTM, CompactPoseBoneToModify, EBoneControlSpace::BCS_ComponentSpace);
					
					NewBoneTM.AddToTranslation(Translation);

					// Convert back to Component Space.
					FAnimationRuntime::ConvertBoneSpaceTransformToCS(ComponentTransform, Output.Pose, NewBoneTM, CompactPoseBoneToModify, EBoneControlSpace::BCS_ComponentSpace);
					
					//OutBoneTransforms.Add( FBoneTransform(BodyShakeBone.GetCompactPoseIndex(BoneContainer), NewBoneTM) );
					OutRes.Add(CompactPoseBoneToModify,NewBoneTM);
				}
			}
		}
	}
//For BodyShake End

//For BodyTwist Start
	{
		bool BodyTwistActive = false;
		FGBWPH_Anim_BodyTwistData BodyTwistData = FGBWPH_Anim_BodyTwistData();
		
		BodyTwistActive = IGBWPHBPI_Anim::Execute_GetBodyTwistData(PHAnimInterfaceComponent,false,BodyTwistData);
		
		if (BodyTwistActive)
		{
			if (BodyTwistTimer>BodyTwistData.GetTotalTime())
			{
				IGBWPHBPI_Anim::Execute_GetBodyTwistData(PHAnimInterfaceComponent,true,BodyTwistData);
				BodyTwistTimer = 0.0f;
			}
			else
			{
				FBoneReference BodyTwistBone = FBoneReference(BodyTwistData.BoneName);
				if (BodyTwistBone.Initialize(BoneContainer))
				{
					FCompactPoseBoneIndex CompactPoseBoneToModify = BodyTwistBone.GetCompactPoseIndex(BoneContainer);
					FTransform NewBoneTM = Output.Pose.GetComponentSpaceTransform(CompactPoseBoneToModify);
					if (OutRes.Find(CompactPoseBoneToModify))
					{
						NewBoneTM = OutRes.FindRef(CompactPoseBoneToModify);
					}
					
					FRotator Rotation = BodyTwistData.Rotation;

					if (BodyTwistData.isIn(BodyTwistTimer))
					{
						Rotation = UKismetMathLibrary::RLerp(FRotator::ZeroRotator,
							BodyTwistData.Rotation,
							BodyTwistTimer/BodyTwistData.InTime,
							true);
					}
					else if (BodyTwistData.isOut(BodyTwistTimer))
					{
						Rotation = UKismetMathLibrary::RLerp(FRotator::ZeroRotator,
							BodyTwistData.Rotation,
							1-(BodyTwistTimer-BodyTwistData.InTime-BodyTwistData.HoldTime)/BodyTwistData.OutTime,
							true);
					}
		
					// Convert to Bone Space.
					FAnimationRuntime::ConvertCSTransformToBoneSpace(ComponentTransform, Output.Pose, NewBoneTM, CompactPoseBoneToModify, EBoneControlSpace::BCS_ComponentSpace);	
					const FQuat BoneQuat(Rotation);
					NewBoneTM.SetRotation(BoneQuat * NewBoneTM.GetRotation());

					// Convert back to Component Space.
					FAnimationRuntime::ConvertBoneSpaceTransformToCS(ComponentTransform, Output.Pose, NewBoneTM, CompactPoseBoneToModify, EBoneControlSpace::BCS_ComponentSpace);
	
					//OutBoneTransforms.Add( FBoneTransform(BodyTwistBone.GetCompactPoseIndex(BoneContainer), NewBoneTM) );
					OutRes.Add(CompactPoseBoneToModify,NewBoneTM);
				}

				BodyTwistTimer+=DeltaTime;
			}
		}
	}
//For BodyTwist End
	
	OutRes.KeySort([](FCompactPoseBoneIndex A, FCompactPoseBoneIndex B) {
		return A < B; // sort keys in reverse
	});
	for (auto& item : OutRes)
	{
		OutBoneTransforms.Add( FBoneTransform(item.Key, item.Value) );
	}

}

bool FGBWPHAnimNode_PostureAdjust::IsValidToEvaluate(const USkeleton* Skeleton, const FBoneContainer& RequiredBones) 
{
	return true;
}

void FGBWPHAnimNode_PostureAdjust::InitializeBoneReferences(const FBoneContainer& RequiredBones) 
{
	
}

bool FGBWPHAnimNode_PostureAdjust::CheckOwner(FComponentSpacePoseContext Context)
{
	if (!OwnerActor)
	{
		OwnerActor = Context.AnimInstanceProxy->GetSkelMeshComponent()->GetOwner();
	}

	if (OwnerActor)
	{
		if (!PHAnimInterfaceComponent)
		{
			TArray<UActorComponent*> Comps = OwnerActor->GetComponentsByInterface(UGBWPHBPI_Anim::StaticClass());

			if (!Comps.IsEmpty())
			{
				PHAnimInterfaceComponent = Comps[0];
			}
		}

		if (PHAnimInterfaceComponent)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

bool FGBWPHAnimNode_PostureAdjust::CheckValid(FComponentSpacePoseContext Context)
{
	if (!CheckOwner(Context))
	{
		return false;
	}

	EWorldType::Type World = OwnerActor->GetWorld()->WorldType;
	return (World==EWorldType::Type::Game || World==EWorldType::Type::PIE);
}

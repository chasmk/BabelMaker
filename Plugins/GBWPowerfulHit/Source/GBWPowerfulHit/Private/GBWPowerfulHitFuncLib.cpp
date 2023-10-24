// Copyright GanBowen 2022-2023. All Rights Reserved.


#include "GBWPowerfulHitFuncLib.h"
#include "GameplayTagsManager.h"
#include "GameFramework/Character.h"

FGameplayTagContainer UGBWPowerfulHitFuncLib::GetDirectChildTags(const FGameplayTag& Tag)
{
	FGameplayTagContainer TagContainer;
	TSharedPtr<FGameplayTagNode> GameplayTagNode = UGameplayTagsManager::Get().FindTagNode(Tag);
	if (GameplayTagNode.IsValid())
	{
		TArray< TSharedPtr<FGameplayTagNode> >& ChildrenNodes = GameplayTagNode->GetChildTagNodes();
		for (TSharedPtr<FGameplayTagNode> ChildNode : ChildrenNodes)
		{
			if (ChildNode.IsValid())
			{
				TagContainer.AddTag(ChildNode->GetCompleteTag());
			}
		}
	}
	return TagContainer;
}

FGameplayTagContainer UGBWPowerfulHitFuncLib::GetChildTagsRecurse(const FGameplayTag& Tag)
{
	return UGameplayTagsManager::Get().RequestGameplayTagChildren(Tag);
}

bool UGBWPowerfulHitFuncLib::ForceDestroyComponent(UActorComponent* ActorComponent)
{
	if (ActorComponent && ActorComponent->IsValidLowLevel())
	{
		ActorComponent->DestroyComponent();
		return true;
	}
	return false;
}

bool UGBWPowerfulHitFuncLib::IsLocalOwn(AActor* Actor)
{
	bool bRes = false;

	if (Actor)
	{
		ENetRole LocalRole = Actor->GetLocalRole();
		ENetRole RemoteRole = Actor->GetRemoteRole();

		if (LocalRole == ENetRole::ROLE_Authority)
		{
			if (RemoteRole == ENetRole::ROLE_None)
			{
				bRes = false;
			}
			else
			{
				bRes = true;
			}
		}
		else
		{
			if (LocalRole == ENetRole::ROLE_AutonomousProxy)
			{
				bRes = true;
			}
			else
			{
				bRes = false;
			}
		}
	}
	else
	{
		bRes = false;
	}
	
	return bRes;
}

AGBWPH_HitTrace_Base* UGBWPowerfulHitFuncLib::GBWMakeHitTraceActor(
	const TSubclassOf<AGBWPH_HitTrace_Base> HitTraceActorClass, 
	FGBWPH_HitTraceParameter HitTraceParameter)
{
	if (HitTraceParameter.Hitter)
	{
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.Owner = HitTraceParameter.Hitter;
		SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		AGBWPH_HitTrace_Base* res
			= HitTraceParameter.Hitter->GetWorld()->SpawnActor<AGBWPH_HitTrace_Base>(HitTraceActorClass,HitTraceParameter.Hitter->GetTransform(),SpawnInfo);
		res->InitHitTrace(HitTraceParameter);
		return res;
	}
	return nullptr;
}

TArray<FName> UGBWPowerfulHitFuncLib::GBWGetSectionNameFromMontage(UAnimMontage* Montage)
{
	TArray<FName> result;
	int maxIndex = Montage->CompositeSections.Num() - 1;
	for (int i = 0; i <= maxIndex; i++)
	{
		result.Add(Montage->CompositeSections[i].SectionName);
	}

	return result;
}

bool UGBWPowerfulHitFuncLib::GBWGetRootMotionDeltaTransform(
	USkeletalMeshComponent* SKMComponent,
	float DeltaTime,
	FTransform& Result)
{
	if(SKMComponent && SKMComponent->GetAnimInstance()->GetCurrentActiveMontage() && DeltaTime > 0)
	{
		const FAnimMontageInstance* MontageInstance = SKMComponent->GetAnimInstance()->GetActiveMontageInstance();
		const float StartTrackPosition = SKMComponent->GetAnimInstance()->Montage_GetPosition(nullptr);
		
		Result = SKMComponent->GetAnimInstance()->GetCurrentActiveMontage()->ExtractRootMotionFromTrackRange(StartTrackPosition, StartTrackPosition+DeltaTime);
		return true;
	}
	return false;
}

bool UGBWPowerfulHitFuncLib::GBWGetRootMotionDataFormMontage(
	UAnimMontage* AnimationMontage,
	const FName SectionName,
	float TimeInterval,
	TArray<float>& TimeArr,
	TArray<FTransform>& TransformArr,
	float& TotalTime)
{
	if (AnimationMontage)
	{
		if (TimeInterval<=0)
		{
			TimeInterval = 1.0f/60.0f;
		}
		int SectionIndex = 0;
		if (AnimationMontage->IsValidSectionName(SectionName))
		{
			SectionIndex = AnimationMontage->GetSectionIndex(SectionName);
		}
		float StartTime = 0.0f;
		float EndTime = 0.0f;
		AnimationMontage->GetSectionStartAndEndTime(SectionIndex, StartTime, EndTime);

		TotalTime = EndTime - StartTime;

		for(float t = StartTime+TimeInterval; t<=EndTime; t+=TimeInterval)
		{
			TimeArr.Add(t);
			TransformArr.Add(AnimationMontage->ExtractRootMotionFromTrackRange(StartTime,t));
		}
	}
	
	return TimeArr.Num()>0;
}

bool UGBWPowerfulHitFuncLib::GBWGetRootMotionMode(
	UAnimInstance* AnimInstance,
	TEnumAsByte<ERootMotionMode::Type>& RootMotionMode)
{
	if (AnimInstance)
	{
		RootMotionMode = AnimInstance->RootMotionMode;
		return true;
	}
	return false;
}

bool UGBWPowerfulHitFuncLib::GBWIsSKMCompatibleWithMontage(USkeletalMeshComponent* Mesh, UAnimMontage* Montage)
{
	if (Mesh && Mesh->SkeletalMesh && Montage)
	{
		if (Mesh->SkeletalMesh->GetSkeleton() && Montage->GetSkeleton())
		{
			return Mesh->SkeletalMesh->GetSkeleton()->IsCompatible(Montage->GetSkeleton());
		}
	}

	return false;
}

bool UGBWPowerfulHitFuncLib::GBWSetAnimInstanceClass(USkeletalMeshComponent* Mesh, UClass* NewClass)
{
	if (Mesh && NewClass!=nullptr)
	{
		// set the animation mode
		Mesh->SetAnimationMode(EAnimationMode::Type::AnimationBlueprint);

		// ReInit
		Mesh->AnimClass = NewClass;
		Mesh->ClearAnimScriptInstance();
		Mesh->InitAnim(true);

		return true;
	}
	
	return false;
}

bool UGBWPowerfulHitFuncLib::GBWSetSkeletalMeshAndAnimInstance(
	USkeletalMeshComponent* SkeletalMeshComponent,
	USkeletalMesh* SkeletalMesh,
	FTransform MeshTransform,
	UClass* AnimInstanceClass,
	bool bReinitPose)
{
	if (SkeletalMeshComponent && SkeletalMesh && AnimInstanceClass!=nullptr)
	{
		SkeletalMeshComponent->SetSkeletalMesh(SkeletalMesh,bReinitPose);
		SkeletalMeshComponent->SetRelativeTransform(MeshTransform);
		return UGBWPowerfulHitFuncLib::GBWSetAnimInstanceClass(SkeletalMeshComponent,AnimInstanceClass);
	}

	return false;
}

bool UGBWPowerfulHitFuncLib::GBWSetAnimRootMotionTranslationScale(
	ACharacter* Character,
	float InAnimRootMotionTranslationScale)
{
	if (Character && InAnimRootMotionTranslationScale>=0)
	{
		Character->SetAnimRootMotionTranslationScale(InAnimRootMotionTranslationScale);
		return true;
	}
	return false;
}

void UGBWPowerfulHitFuncLib::GBWCloneObject(UObject* ObjectIn, UObject*& Object)
{
	Object = DuplicateObject<UObject>(ObjectIn, Object);
}

bool UGBWPowerfulHitFuncLib::GBWSerializeObject(UObject* ObjectIn, FGBWPHSerializationObjectInfo& SerializationObjectInfo)
{
	FGBWPHSerializationObjectInfo res = FGBWPHSerializationObjectInfo();
	res.ObjectGuid = FGuid::NewGuid();
	if (ObjectIn)
	{
		res.ObjectClass = ObjectIn->GetClass();
		FObjectWriter ObjectWriter = FObjectWriter(ObjectIn, res.ObjectBytes);
		SerializationObjectInfo = res;
		return true;
	}
	SerializationObjectInfo = res;
	return false;
}

bool UGBWPowerfulHitFuncLib::GBWDeserializeObject(UObject* Outer, FGBWPHSerializationObjectInfo SerializationObjectInfo, UObject*& Object)
{
	if(SerializationObjectInfo.ObjectClass && Outer)
	{
		Object = NewObject<UObject>(Outer, SerializationObjectInfo.ObjectClass);
		FObjectReader ObjectReader = FObjectReader(Object, SerializationObjectInfo.ObjectBytes);
		return true;
	}
	return false;
}

bool UGBWPowerfulHitFuncLib::GBWDivideSerializationObject(
	FGBWPHSerializationObjectInfo SerializationObjectInfo,
	int FragmentLength,
	FGBWPHSerializationObjectInfoFragmentSet& SerializationObjectInfoFragmentSet)
{
	FGBWPHSerializationObjectInfoFragmentSet res = FGBWPHSerializationObjectInfoFragmentSet();
	if (SerializationObjectInfo.ObjectGuid.IsValid() && IsValid(SerializationObjectInfo.ObjectClass))
	{
		int BytesLength = SerializationObjectInfo.ObjectBytes.Num();
		int FragmentLength_ = FragmentLength;
		if (FragmentLength_<=0 || FragmentLength_>BytesLength)
		{
			FragmentLength_ = BytesLength;
		}
		
		res.ObjectClass = SerializationObjectInfo.ObjectClass;
		res.ObjectGuid = SerializationObjectInfo.ObjectGuid;
		res.TotalLength = UKismetMathLibrary::FCeil(static_cast<double>(BytesLength)/static_cast<double>(FragmentLength_));

		for (int i=0; i<res.TotalLength; i++)
		{
			FGBWPHSerializationObjectInfoFragment NewFragment = FGBWPHSerializationObjectInfoFragment();
			NewFragment.ObjectGuid = res.ObjectGuid;
			NewFragment.Index = i;
			NewFragment.ObjectBytes.Empty();

			const int Start = i*FragmentLength_;
			const int End = UKismetMathLibrary::Min(Start+FragmentLength_,BytesLength);
			for (int j=Start;j<End;j++)
			{
				NewFragment.ObjectBytes.Add(SerializationObjectInfo.ObjectBytes[j]);
			}

			res.ObjectInfoFragments.Add(NewFragment);
		}
		
		SerializationObjectInfoFragmentSet = res;
		return true;
	}
	SerializationObjectInfoFragmentSet = res;
	return false;
}

bool UGBWPowerfulHitFuncLib::GBWRebuildSerializationObject(
	FGBWPHSerializationObjectInfoFragmentSet SerializationObjectInfoFragmentSet,
	FGBWPHSerializationObjectInfo& SerializationObjectInfo)
{
	FGBWPHSerializationObjectInfo Res = FGBWPHSerializationObjectInfo();

	if (SerializationObjectInfoFragmentSet.ObjectGuid.IsValid()
		&& IsValid(SerializationObjectInfoFragmentSet.ObjectClass)
		&& SerializationObjectInfoFragmentSet.ObjectInfoFragments.Num()==SerializationObjectInfoFragmentSet.TotalLength)
	{
		Res.ObjectClass = SerializationObjectInfoFragmentSet.ObjectClass;
		Res.ObjectGuid = SerializationObjectInfoFragmentSet.ObjectGuid;
		
		SerializationObjectInfoFragmentSet.ObjectInfoFragments.Sort(
			[](const FGBWPHSerializationObjectInfoFragment& A, const FGBWPHSerializationObjectInfoFragment& B)
			{
				return A.Index < B.Index;
			});

		for (FGBWPHSerializationObjectInfoFragment ObjectInfoFragment : SerializationObjectInfoFragmentSet.ObjectInfoFragments)
		{
			Res.ObjectBytes.Append(ObjectInfoFragment.ObjectBytes);
		}
		
		SerializationObjectInfo = Res;
		return true;
	}
	
	SerializationObjectInfo = Res;
	return false;
}

bool UGBWPowerfulHitFuncLib::GBWSerializeObjectToFragmentSet(
	UObject* ObjectIn,
	int FragmentLength,
	FGBWPHSerializationObjectInfoFragmentSet& SerializationObjectInfoFragmentSet)
{
	FGBWPHSerializationObjectInfo SerializationObjectInfo = FGBWPHSerializationObjectInfo();
	if (GBWSerializeObject(ObjectIn,SerializationObjectInfo))
	{
		return GBWDivideSerializationObject(SerializationObjectInfo,FragmentLength,SerializationObjectInfoFragmentSet);
	}

	SerializationObjectInfoFragmentSet = FGBWPHSerializationObjectInfoFragmentSet();
	return false;
}

bool UGBWPowerfulHitFuncLib::GBWDeserializeObjectFromFragmentSet(
	UObject* Outer,
	FGBWPHSerializationObjectInfoFragmentSet SerializationObjectInfoFragmentSet,
	UObject*& Object)
{
	FGBWPHSerializationObjectInfo SerializationObjectInfo = FGBWPHSerializationObjectInfo();
	if (GBWRebuildSerializationObject(SerializationObjectInfoFragmentSet,SerializationObjectInfo))
	{
		return GBWDeserializeObject(Outer,SerializationObjectInfo,Object);
	}

	Object = nullptr;
	return false;
}

FTransform UGBWPowerfulHitFuncLib::GetComponentSpaceTransformRefPose(
	USkeletalMeshComponent* MeshComponent,
	FName BoneName)
{
	const FReferenceSkeleton& RefSkeleton = MeshComponent->SkeletalMesh->GetRefSkeleton();
	return FAnimationRuntime::GetComponentSpaceTransformRefPose(RefSkeleton, MeshComponent->GetBoneIndex(BoneName));
}

USkeletalMeshComponent* UGBWPowerfulHitFuncLib::AddSkeletalMeshComponent(
	AActor* Actor,
	FName InBaseName,
	USkeletalMesh* InSkelMesh,
	USceneComponent* ParentComponentAttachTo,
	FName SocketNameAttachTo,
	USkeletalMeshComponent* MasterPoseComponent,
	FName ComponentTagToAdd)
{
	const FName MeshName = MakeUniqueObjectName(Actor, USkeletalMeshComponent::StaticClass(), InBaseName);
	USkeletalMeshComponent* NewMesh = NewObject<USkeletalMeshComponent>(Actor, MeshName);
	NewMesh->RegisterComponent();
	NewMesh->SetSkeletalMesh(InSkelMesh);
	Actor->AddInstanceComponent(NewMesh);
	NewMesh->AttachToComponent(ParentComponentAttachTo, FAttachmentTransformRules::SnapToTargetIncludingScale, SocketNameAttachTo);
	NewMesh->SetMasterPoseComponent(MasterPoseComponent, false);
	NewMesh->BindClothToMasterPoseComponent();
	NewMesh->ComponentTags.Add(ComponentTagToAdd);

	return NewMesh;
}

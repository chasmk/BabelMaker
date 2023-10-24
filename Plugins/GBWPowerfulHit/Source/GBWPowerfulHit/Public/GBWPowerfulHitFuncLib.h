// Copyright GanBowen 2022-2023. All Rights Reserved.

#pragma once

#include "Runtime/Launch/Resources/Version.h"
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Animation/AnimMontage.h"
#include <GBWPowerfulHitNetWorkObject.h>
#include "Serialization/ObjectWriter.h"
#include "Serialization/ObjectReader.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Misc/Guid.h"
#include "GameplayTagContainer.h"

#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION > 0
#include "Engine/ScopedMovementUpdate.h"
#endif

//For Scratch Start
#include "Components/SkeletalMeshComponent.h"
#include "AnimationRuntime.h"
//For Scratch End
#include "GBWPowerfulHitFuncLib.generated.h"

USTRUCT(BlueprintType, meta = (DisplayName = "PH_SerializationObjectInfo"))
struct FGBWPHSerializationObjectInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ObjectInfo")
	FGuid ObjectGuid = FGuid();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ObjectInfo")
	UClass* ObjectClass = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ObjectInfo")
	TArray<uint8> ObjectBytes;

public:
	FGBWPHSerializationObjectInfo()
	{
		ObjectBytes.Empty();
	}
};
USTRUCT(BlueprintType, meta = (DisplayName = "PH_SerializationObjectInfo_Fragment"))
struct FGBWPHSerializationObjectInfoFragment
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ObjectInfo")
	FGuid ObjectGuid = FGuid();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ObjectInfo")
	int Index = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ObjectInfo")
	TArray<uint8> ObjectBytes;

public:
	FGBWPHSerializationObjectInfoFragment()
	{
		ObjectBytes.Empty();
	}
};
USTRUCT(BlueprintType, meta = (DisplayName = "PH_SerializationObjectInfo_FragmentSet"))
struct FGBWPHSerializationObjectInfoFragmentSet
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ObjectInfo")
	FGuid ObjectGuid = FGuid();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ObjectInfo")
	UClass* ObjectClass = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ObjectInfo")
	int TotalLength = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ObjectInfo")
	TArray<FGBWPHSerializationObjectInfoFragment> ObjectInfoFragments;

public:
	FGBWPHSerializationObjectInfoFragmentSet()
	{
		ObjectInfoFragments.Empty();
	}
};

UCLASS()
class GBWPOWERFULHIT_API UGBWPowerfulHitFuncLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
//For CommonTools Start
	UFUNCTION(BlueprintPure, Category = "GBW|PowerfulHit|GameplayTag", Meta = (AutoCreateRefTerm = "Tag"))
		static FGameplayTagContainer GetDirectChildTags(const FGameplayTag& Tag);
	UFUNCTION(BlueprintPure, Category = "GBW|PowerfulHit|GameplayTag", Meta = (AutoCreateRefTerm = "Tag"))
		static FGameplayTagContainer GetChildTagsRecurse(const FGameplayTag& Tag);

	UFUNCTION(BlueprintCallable, Category = "GBW|PowerfulHit")
		static bool ForceDestroyComponent(UActorComponent* ActorComponent);

	UFUNCTION(BlueprintCallable, Category = "GBW|PowerfulHit|Net")
		static bool IsLocalOwn(AActor* Actor);
//For CommonTools End

//For HitTrace Start
	UFUNCTION(BlueprintCallable, Category = "GBW|PowerfulHit|HitTrace", Meta = (DisplayName = "MakeHitTraceActor"))
		static AGBWPH_HitTrace_Base* GBWMakeHitTraceActor(const TSubclassOf<AGBWPH_HitTrace_Base> HitTraceActorClass, FGBWPH_HitTraceParameter HitTraceParameter);
//For HitTrace End

//For Anim Start
	UFUNCTION(BlueprintCallable, Category = "GBW|PowerfulHit|Animation", meta = (DisplayName = "GetSectionNameFromMontage"))
		static TArray<FName> GBWGetSectionNameFromMontage(UAnimMontage* Montage);
	
	UFUNCTION(BlueprintCallable, Category = "GBW|PowerfulHit|Animation", meta = (DisplayName = "GetRootMotionDeltaTransform"))
		static bool GBWGetRootMotionDeltaTransform(
			USkeletalMeshComponent* SKMComponent,
			float DeltaTime,
			FTransform& Result);
	UFUNCTION(BlueprintCallable, Category = "GBW|PowerfulHit|Animation", meta = (DisplayName = "GetRootMotionDataFormMontage"))
		static bool GBWGetRootMotionDataFormMontage(
			UAnimMontage* AnimationMontage,
			const FName SectionName,
			float TimeInterval,
			TArray<float>& TimeArr,
			TArray<FTransform>& TransformArr,
			float& TotalTime);
	UFUNCTION(BlueprintCallable, Category = "GBW|PowerfulHit|Animation", meta = (DisplayName = "GetRootMotionMode"))
			static bool GBWGetRootMotionMode(
				UAnimInstance* AnimInstance,
				TEnumAsByte<ERootMotionMode::Type>& RootMotionMode);
	UFUNCTION(BlueprintCallable, Category = "GBW|PowerfulHit|Animation", meta = (DisplayName = "IsSKMCompatibleWithMontage"))
		static bool GBWIsSKMCompatibleWithMontage(
			USkeletalMeshComponent* Mesh,
			UAnimMontage* Montage);
	UFUNCTION(BlueprintCallable, Category = "GBW|PowerfulHit|Animation", meta = (DisplayName = "SetAnimInstanceClass"))
		static bool GBWSetAnimInstanceClass(
			USkeletalMeshComponent* Mesh,
			UClass* NewClass);
	UFUNCTION(BlueprintCallable, Category = "GBW|PowerfulHit|Animation", meta = (DisplayName = "SetSkeletalMeshAndAnimInstance"))
    	static bool GBWSetSkeletalMeshAndAnimInstance(
    		USkeletalMeshComponent* SkeletalMeshComponent,
    		USkeletalMesh* SkeletalMesh,
    		FTransform MeshTransform,
    		UClass* AnimInstance,
    		bool bReinitPose = false);
	UFUNCTION(BlueprintCallable, Category = "GBW|PowerfulHit|Animation", meta = (DisplayName = "SetAnimRootMotionTranslationScale"))
		static bool GBWSetAnimRootMotionTranslationScale(
			ACharacter* Character,
			float InAnimRootMotionTranslationScale = 1.f);
//For Anim End

//For Object Start
	UFUNCTION(BlueprintPure,
		Category = "GBW|PowerfulHit|Tools",
		Meta = (DisplayName = "CloneObject",
			Keywords = "CloneObject",
			DeterminesOutputType = "Class",
			DynamicOutputParam = "Object"))
		static void GBWCloneObject(UObject* ObjectIn, UObject*& Object);

	UFUNCTION(BlueprintCallable,
		Category = "GBW|PowerfulHit|Tools",
		Meta = (DisplayName = "SerializeObject",
			Keywords = "SerializeObject"))
		static bool GBWSerializeObject(UObject* ObjectIn, FGBWPHSerializationObjectInfo& SerializationObjectInfo);
	
	UFUNCTION(BlueprintCallable,
		Category = "GBW|PowerfulHit|Tools",
		Meta = (DisplayName = "DeserializeObject",
			Keywords = "DeserializeObject"))
		static bool GBWDeserializeObject(UObject* Outer, FGBWPHSerializationObjectInfo SerializationObjectInfo, UObject*& Object);

	UFUNCTION(BlueprintCallable,
		Category = "GBW|PowerfulHit|Tools",
		Meta = (DisplayName = "DivideSerializationObject",
			Keywords = "DivideSerializationObject"))
		static bool GBWDivideSerializationObject(
			FGBWPHSerializationObjectInfo SerializationObjectInfo,
			int FragmentLength,
			FGBWPHSerializationObjectInfoFragmentSet& SerializationObjectInfoFragmentSet);

	UFUNCTION(BlueprintCallable,
		Category = "GBW|PowerfulHit|Tools",
		Meta = (DisplayName = "RebuildSerializationObject",
			Keywords = "RebuildSerializationObject"))
		static bool GBWRebuildSerializationObject(
			FGBWPHSerializationObjectInfoFragmentSet SerializationObjectInfoFragmentSet,
			FGBWPHSerializationObjectInfo& SerializationObjectInfo);

	UFUNCTION(BlueprintCallable,
			Category = "GBW|PowerfulHit|Tools",
			Meta = (DisplayName = "SerializeObjectToFragmentSet",
			Keywords = "SerializeObjectToFragmentSet"))
	static bool GBWSerializeObjectToFragmentSet(
		UObject* ObjectIn,
		int FragmentLength,
		FGBWPHSerializationObjectInfoFragmentSet& SerializationObjectInfoFragmentSet);

	UFUNCTION(BlueprintCallable,
		Category = "GBW|PowerfulHit|Tools",
		Meta = (DisplayName = "DeserializeObjectFromFragmentSet",
			Keywords = "DeserializeObjectFromFragmentSet"))
		static bool GBWDeserializeObjectFromFragmentSet(
			UObject* Outer,
			FGBWPHSerializationObjectInfoFragmentSet SerializationObjectInfoFragmentSet,
			UObject*& Object);
//For Object End

//For Scratch Start
	UFUNCTION(BlueprintCallable, Category = "GBW|PowerfulHit|Scratch|Tools")
		static FTransform GetComponentSpaceTransformRefPose(USkeletalMeshComponent* MeshComponent, FName BoneName);

	UFUNCTION(BlueprintCallable, Category = "GBW|PowerfulHit|Scratch|Tools")
		static USkeletalMeshComponent* AddSkeletalMeshComponent(
			AActor* Actor,
			FName InBaseName,
			USkeletalMesh* InSkelMesh,
			USceneComponent* ParentComponentAttachTo,
			FName SocketNameAttachTo,
			USkeletalMeshComponent* MasterPoseComponent,
			FName ComponentTagToAdd);
//For Scratch End
};

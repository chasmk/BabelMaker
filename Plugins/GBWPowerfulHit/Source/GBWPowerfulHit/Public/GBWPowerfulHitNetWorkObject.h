// Copyright GanBowen 2022-2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GBW_HitBoxComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "UObject/NoExportTypes.h"
#include "GBWPowerfulHitNetWorkObject.generated.h"


class UGBWPH_ConditionGeneration;

UENUM(BlueprintType)
enum class EGBWPHTargetType :uint8
{
	Self UMETA(DisplayName = "Self"),
	Opponent UMETA(DisplayName = "Opponent"),
	Both  UMETA(DisplayName = "Both")
};

UENUM(BlueprintType)
enum class EGBWTrailWidthMode :uint8
{
	FromCentre UMETA(DisplayName = "From Centre"),
	FromFirst UMETA(DisplayName = "From First Socket"),
	FromSecond UMETA(DisplayName = "From Second Socket")
};

UENUM(BlueprintType)
enum class EGBWMoveType :uint8
{
	AwayFromOpponent UMETA(DisplayName = "AwayFromOpponent"),
	CloseToOppnent UMETA(DisplayName = "CloseToOppnent"),
	UP UMETA(DisplayName = "UP")
};

UENUM(BlueprintType)
enum class EGBWApplyType :uint8
{
	Overlap UMETA(DisplayName = "Overlap"),
	Accrual UMETA(DisplayName = "Accrual")
};

UENUM(BlueprintType)
enum class EGBWConditionSetEvaluationMode :uint8
{
	AllMatch UMETA(DisplayName = "AllMatch"),
	AnyMatch UMETA(DisplayName = "AnyMatch"),
	AllNotMatch UMETA(DisplayName = "AllNotMatch"),
	AnyNotMatch UMETA(DisplayName = "AnyNotMatch")
};

USTRUCT(BlueprintType, meta = (DisplayName = "PH_ConditionSet"))
struct FGBWPHConditionSet
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ConditionSet")
		EGBWConditionSetEvaluationMode EvaluationMode = EGBWConditionSetEvaluationMode::AllMatch;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, Category = "ConditionSet", meta = (MustImplement = GBWPHInterface_Condition))
		TMap<FString, UGBWPH_ConditionGeneration*> ConditionSet;
};

UCLASS(Blueprintable, DefaultToInstanced, EditInlineNew, meta = (DisplayName = "GBW_PowerfulHitNetWorkObject"))
class GBWPOWERFULHIT_API UGBWPowerfulHitNetWorkObject : public UObject
{
	GENERATED_BODY()

	virtual bool IsNameStableForNetworking() const override
	{
		return true;
	}

	virtual bool IsSupportedForNetworking() const override
	{
		return true;
	}

protected:
	struct FWorldContext* WorldContext;

public:

	UGBWPowerfulHitNetWorkObject(){};

	// Allows the Object to use BP_Functions
	UFUNCTION(BlueprintCallable, Category="WorldContext")
		void SetWorldContext(UObject* NewWorldContext);

	UFUNCTION(BlueprintCallable, Category = "WorldContext")
		UObject* GetWorldContextObject();

	//~ Begin UObject Interface
	virtual class UWorld* GetWorld() const override final;
	struct FWorldContext* GetWorldContext() const { return WorldContext; };
};

UCLASS(Blueprintable, DefaultToInstanced, EditInlineNew, meta = (DisplayName = "GBWPH_ConditionGeneration_Base"))
class GBWPOWERFULHIT_API UGBWPH_ConditionGeneration : public UGBWPowerfulHitNetWorkObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, Category = "GBW|PH|Condition")
		bool GBW_GetConditionGenerationResult(AActor* SelfActor, AActor* OpponentActor,const FHitResult HitResult) const;
	virtual bool GBW_GetConditionGenerationResult_Implementation(AActor* SelfActor, AActor* OpponentActor,const FHitResult HitResult) const {
		return true;
	};

	UFUNCTION(BlueprintCallable, Category = "GBW|PH|Condition", meta = (DisplayName = "GetConditionResult"))
		bool GetConditionGenerationResult(AActor* SelfActor, AActor* OpponentActor, const FHitResult HitResult) const {
		return GBW_GetConditionGenerationResult(SelfActor, OpponentActor, HitResult);
	};
};

class UGBWPH_Feature_Base;
UCLASS(Blueprintable, DefaultToInstanced, EditInlineNew, meta = (DisplayName = "GBWPH_FeatureConfig_Base"))
class GBWPOWERFULHIT_API UGBWPH_FeatureConfig_Base : public UGBWPowerfulHitNetWorkObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Target")
		EGBWPHTargetType TargetActor = EGBWPHTargetType::Self;

private:
	UPROPERTY(EditDefaultsOnly, Category = "InstanceMode")
		bool IsSingletonMode = false;

public:
	
	UFUNCTION(BlueprintPure, Category = "GBW|PH|Feature", meta = (DisplayName = "IsSingletonMode"))
		bool GetSingletonMode() {
		return IsSingletonMode;
	};

	UFUNCTION(BlueprintNativeEvent, Category = "GBW|PH|Feature")
		bool GBW_ExecuteFeature(AActor* OwnerActor, 
			AActor* SelfActor,
			AActor* OpponentActor, 
			FHitResult HitResult, 
			UGBWPH_Feature_Base* ExtantPHFExecutor, 
			UGBWPH_Feature_Base* &PHFExecutor);
	virtual bool GBW_ExecuteFeature_Implementation(AActor* OwnerActor, 
		AActor* SelfActor,
		AActor* OpponentActor, 
		FHitResult HitResult, 
		UGBWPH_Feature_Base* ExtantPHFExecutor, 
		UGBWPH_Feature_Base*& PHFExecutor) {
		return true;
	};

	UFUNCTION(BlueprintCallable, Category = "GBW|PH|Feature", meta = (DisplayName = "ExecuteFeature"))
		bool ExecuteFeature(AActor* OwnerActor, 
			AActor* SelfActor,
			AActor* OpponentActor, 
			FHitResult HitResult,
			UGBWPH_Feature_Base* ExtantPHFExecutor,
			UGBWPH_Feature_Base*& PHFExecutor) {
		return GBW_ExecuteFeature(OwnerActor,
			SelfActor,
			OpponentActor, 
			HitResult, 
			ExtantPHFExecutor,
			PHFExecutor);
	};
};

UCLASS(Blueprintable, DefaultToInstanced, EditInlineNew, meta = (DisplayName = "GBWPH_Feature_Base"))
class GBWPOWERFULHIT_API UGBWPH_Feature_Base : public UGBWPowerfulHitNetWorkObject
{
	GENERATED_BODY()

private:
	UGBWPH_FeatureConfig_Base* ParamObject = nullptr;

public:

	UFUNCTION(BlueprintCallable, Category = "GBW|PH|Feature", meta = (DisplayName = "SetParamObject"))
		void SetParamObject(UGBWPH_FeatureConfig_Base* TheParamObject) {
		ParamObject = TheParamObject;
	};

	UFUNCTION(BlueprintPure, Category = "GBW|PH|Feature", meta = (DisplayName = "GetParamObject"))
		UGBWPH_FeatureConfig_Base* GetParamObject() {
		return ParamObject;
	};

	UFUNCTION(BlueprintNativeEvent, Category = "GBW|PH|Feature")
		bool GBW_ExecuteFeature(AActor* OwnerActor, 
			AActor* SelfActor,
			AActor* OpponentActor, 
			EGBWPHTargetType TargetActor, 
			FHitResult HitResult, 
			UGBWPH_FeatureConfig_Base* TheParamObject);
	virtual bool GBW_ExecuteFeature_Implementation(AActor* OwnerActor, 
		AActor* SelfActor,
		AActor* OpponentActor, 
		EGBWPHTargetType TargetActor, 
		FHitResult HitResult, 
		UGBWPH_FeatureConfig_Base* TheParamObject){
		return true;
	};

	UFUNCTION(BlueprintCallable, Category = "GBW|PH|Feature", meta = (DisplayName = "ExecuteFeature"))
		bool ExecuteFeature(AActor* OwnerActor, 
			AActor* SelfActor,
			AActor* OpponentActor, 
			EGBWPHTargetType TargetActor, 
			FHitResult HitResult, 
			UGBWPH_FeatureConfig_Base* TheParamObject){
		return GBW_ExecuteFeature(OwnerActor, SelfActor, OpponentActor, TargetActor, HitResult, TheParamObject);
	};

	UFUNCTION(BlueprintCallable, Category = "GBox|PH|Feature", meta = (DisplayName = "Destroy"))
		void Destroy()
	{
		if (this && this != nullptr)
		{
			this->ConditionalBeginDestroy();
		}
	};
};

USTRUCT(BlueprintType, meta = (DisplayName = "GBW_PowerfulHitFeatureSet"))
struct FGBWPowerfulHitFeatureSet
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, Instanced, EditAnywhere, Category = "FeatureSet")
		TArray<UGBWPH_FeatureConfig_Base*> FeatureSet;

	FGBWPowerfulHitFeatureSet() {};
};

UCLASS(Blueprintable, DefaultToInstanced, EditInlineNew, meta = (DisplayName = "GBWPH_Script_Base"))
class GBWPOWERFULHIT_API UGBWPH_Script_Base : public UGBWPowerfulHitNetWorkObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, Category = "GBW|PH|Script")
	bool Execute(AActor* TargetActor, 
		AActor* SelfActor,
		AActor* OpponentActor, 
		FHitResult HitResult, 
		bool TargetActorIsSelf,
		int PreScriptIndex,
		int& NextScriptIndex);
	virtual bool Execute_Implementation(AActor* TargetActor, 
		AActor* SelfActor,
		AActor* OpponentActor, 
		FHitResult HitResult, 
		bool TargetActorIsSelf,
		int PreScriptIndex,
		int& NextScriptIndex){
		NextScriptIndex = PreScriptIndex+1;
		return true;
	};

	UFUNCTION(BlueprintCallable, Category = "GBW|PH|Script", meta = (DisplayName = "ExecuteScript"))
	bool ExecuteScript(AActor* TargetActor, 
		AActor* SelfActor,
		AActor* OpponentActor, 
		FHitResult HitResult, 
		bool TargetActorIsSelf,
		int PreScriptIndex,
		int& NextScriptIndex){
		return Execute(TargetActor, SelfActor, OpponentActor, HitResult, TargetActorIsSelf,PreScriptIndex,NextScriptIndex);
	};

	UFUNCTION(BlueprintCallable, Category = "GBox|PH|Script", meta = (DisplayName = "Destroy"))
		void Destroy()
	{
		if (this && this != nullptr)
		{
			this->ConditionalBeginDestroy();
		}
	};
};

USTRUCT(BlueprintType, meta = (DisplayName = "GBWPH_ScriptSet"))
struct FGBWPHScriptSet
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, Instanced, EditAnywhere, Category = "ScriptSet")
		TArray<UGBWPH_Script_Base*> ScriptSet;

	FGBWPHScriptSet() {};
};

UENUM(BlueprintType)
enum class EGBWPHAdditionalItemType :uint8
{
	None UMETA(DisplayName = "None"),
	SkeletalMesh UMETA(DisplayName = "SkeletalMesh"),
	StaticMesh UMETA(DisplayName = "StaticMesh"),
	Actor  UMETA(DisplayName = "Actor")
};

class UGBWPH_AdditionalItem;
USTRUCT(BlueprintType, meta = (DisplayName = "GBWPH_AdditionalItemSet"))
struct FGBWPHAdditionalItemSet
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, Instanced, EditAnywhere, Category = "AdditionalItemSet")
	TArray<UGBWPH_AdditionalItem*> AdditionalItemSet;

	FGBWPHAdditionalItemSet() {};
};
USTRUCT(BlueprintType, meta = (DisplayName = "GBWPH_AttachSettings"))
struct FGBWPHAttachSettings
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|AttachSettings")
	FTransform ItemTransform = FTransform();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|AttachSettings")
	FName SocketName = NAME_None;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|AttachSettings")
	EAttachmentRule LocationRule = EAttachmentRule::KeepRelative;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|AttachSettings")
	EAttachmentRule RotationRule = EAttachmentRule::KeepRelative;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|AttachSettings")
	EAttachmentRule ScaleRule = EAttachmentRule::KeepRelative;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|AttachSettings")
	bool WeldSimulatedBodies = true;

	FGBWPHAttachSettings() {};
};
USTRUCT(BlueprintType, meta = (DisplayName = "GBWPH_TextTagSettings"))
struct FGBWPHTextTagSettings
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|TextTagSettings")
	FName SocketTag = NAME_None;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|TextTagSettings")
	FName RealTag = NAME_None;

	FGBWPHTextTagSettings() {};
};

UCLASS(Blueprintable, DefaultToInstanced, EditInlineNew, meta = (DisplayName = "AdditionalItem"))
class GBWPOWERFULHIT_API UGBWPH_AdditionalItem : public UGBWPowerfulHitNetWorkObject
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	EGBWPHAdditionalItemType ItemType = EGBWPHAdditionalItemType::None;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings",
	meta = (EditCondition = "ItemType == EGBWPHAdditionalItemType::SkeletalMesh"
	, EditConditionHides))
	USkeletalMesh* SkeletalMesh = nullptr;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings",
		meta = (EditCondition = "ItemType == EGBWPHAdditionalItemType::SkeletalMesh"
		, EditConditionHides))
	TSubclassOf<class UAnimInstance> AnimInstanceClass = nullptr;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings",
		meta = (EditCondition = "ItemType == EGBWPHAdditionalItemType::SkeletalMesh"
		, EditConditionHides))
	bool ReinitPose = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings",
		meta = (EditCondition = "ItemType == EGBWPHAdditionalItemType::StaticMesh"
		, EditConditionHides))
	UStaticMesh* StaticMesh = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings",
		meta = (EditCondition = "ItemType == EGBWPHAdditionalItemType::Actor"
		, EditConditionHides))
	TSubclassOf<class AActor> ActorClass = nullptr;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings",
		meta = (EditCondition = "ItemType == EGBWPHAdditionalItemType::Actor"
		, EditConditionHides))
	bool TryActiveHitBox = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings",
		meta = (EditCondition = "ItemType != EGBWPHAdditionalItemType::None"
		, EditConditionHides))
	FGBWPHAttachSettings AttachSettings = FGBWPHAttachSettings();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings",
		meta = (EditCondition = "ItemType != EGBWPHAdditionalItemType::None"
		, EditConditionHides))
	FName WithTag = NAME_None;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings",
    	meta = (EditCondition = "ItemType != EGBWPHAdditionalItemType::None"
    	, EditConditionHides))
    TArray<FGBWPHTextTagSettings> AdditionalTags;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings",
		meta = (EditCondition = "ItemType != EGBWPHAdditionalItemType::None"
		, EditConditionHides))
	TArray<FGBWPHAdditionalItemSet> ChildItemSet;
};

class AGBWPH_HitTrace_Base;
UCLASS(Blueprintable, DefaultToInstanced, EditInlineNew, meta = (DisplayName = "GBWPH_HitTraceConfig_Base"))
class GBWPOWERFULHIT_API UGBWPH_HitTraceConfig_Base : public UGBWPowerfulHitNetWorkObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "HitTrace")
	TSubclassOf<AGBWPH_HitTrace_Base> HitTraceActorClass;
};
USTRUCT(BlueprintType, meta = (DisplayName = "GBWPH_HitTraceConfigStruct"))
struct FGBWPHHitTraceConfigStruct
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, Instanced, EditAnywhere, Category = "HitTraceConfig")
	UGBWPH_HitTraceConfig_Base* HitTraceConfig = nullptr;

	FGBWPHHitTraceConfigStruct():
	HitTraceConfig(nullptr)
	{};
};
USTRUCT(BlueprintType, meta = (DisplayName = "GBWPH_HitTraceParameter"))
struct FGBWPH_HitTraceParameter
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "HitTrace")
	AActor* Hitter = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "HitTrace")
	UGBW_HitBoxComponent* HitBox = nullptr;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "HitTrace")
	TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjectTypes;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "HitTrace")
	TArray<TEnumAsByte<ETraceTypeQuery>> TraceChannels;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "HitTrace")
	FGBWPHConditionSet HitConditionSet;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "HitTrace")
	float Duration = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "HitTrace")
	UGBWPH_HitTraceConfig_Base* ParameterObject = nullptr;

	FGBWPH_HitTraceParameter(){};
};
USTRUCT(BlueprintType, meta = (DisplayName = "GBWPH_HitTraceResult"))
struct FGBWPH_HitTraceResult
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "HitTrace")
	TArray<FHitResult> HitResults;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "HitTrace")
	TArray<FHitResult> ExtendHitResults;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "HitTrace")
	UObject* ParameterObject = nullptr;

	FGBWPH_HitTraceResult() {};
};
UCLASS(Blueprintable, meta = (DisplayName = "GBWPH_HitTrace_Base"))
class GBWPOWERFULHIT_API AGBWPH_HitTrace_Base : public AActor
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintNativeEvent, Category = "GBW|PH|HitTrace")
	bool GBW_InitHitTrace(
		FGBWPH_HitTraceParameter HitTraceParameter);
	virtual bool GBW_InitHitTrace_Implementation(
		FGBWPH_HitTraceParameter HitTraceParameter){
		return true;
	};

	UFUNCTION(BlueprintCallable, Category = "GBW|PH|HitTrace", meta = (DisplayName = "InitHitTrace"))
	bool InitHitTrace(
		FGBWPH_HitTraceParameter HitTraceParameter){
		return GBW_InitHitTrace(HitTraceParameter);
	};

	UFUNCTION(BlueprintNativeEvent, Category = "GBW|PH|HitTrace")
	bool GBW_IsValid();
	virtual bool GBW_IsValid_Implementation(){
		return false;
	};

	UFUNCTION(BlueprintCallable, Category = "GBW|PH|HitTrace", meta = (DisplayName = "IsValid"))
	bool IsValid(){
		return GBW_IsValid();
	};

	UFUNCTION(BlueprintNativeEvent, Category = "GBW|PH|HitTrace")
	bool GBW_StopHitTrace();
	virtual bool GBW_StopHitTrace_Implementation(){
		return true;
	};

	UFUNCTION(BlueprintCallable, Category = "GBW|PH|HitTrace", meta = (DisplayName = "StopHitTrace"))
	bool StopHitTrace(){
		return GBW_StopHitTrace();
	};
	
	UFUNCTION(BlueprintNativeEvent, Category = "GBW|PH|HitTrace")
	bool GBW_UpdateHitTrace(
		float DeltaTime,
		FGBWPH_HitTraceParameter HitTraceParameter,
		FGBWPH_HitTraceResult& HitTraceResult);
	virtual bool GBW_UpdateHitTrace_Implementation(
		float DeltaTime,
		FGBWPH_HitTraceParameter HitTraceParameter,
		FGBWPH_HitTraceResult& HitTraceResult){
		return true;
	};

	UFUNCTION(BlueprintCallable, Category = "GBW|PH|HitTrace", meta = (DisplayName = "UpdateHitTrace"))
	bool UpdateHitTrace(
		float DeltaTime,
		FGBWPH_HitTraceParameter HitTraceParameter,
		FGBWPH_HitTraceResult& HitTraceResult){
		return GBW_UpdateHitTrace(DeltaTime, HitTraceParameter,HitTraceResult);
	};
};


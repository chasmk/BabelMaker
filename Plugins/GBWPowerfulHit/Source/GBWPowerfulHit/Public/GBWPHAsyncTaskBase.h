// Copyright GanBowen 2022-2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Kismet/KismetSystemLibrary.h"
#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/NoExportTypes.h"
#include "GBWPowerfulHitNetWorkObject.h"
#include "TimerManager.h"
#include "Tickable.h"
#include "GameFramework/RootMotionSource.h"
#include "GBWPHAsyncTaskBase.generated.h"

class UGBWPHAsyncActionObject;

DECLARE_DELEGATE_OneParam(FAsyncActionDelegate, UGBWPHAsyncActionObject*);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAsyncTaskDelegate, UGBWPHAsyncActionObject*, ActionObj);

UCLASS(BlueprintType, Blueprintable)
class UGBWPHAsyncActionObject : public UGBWPowerfulHitNetWorkObject
{
	GENERATED_BODY()
public:
	UGBWPHAsyncActionObject(const FObjectInitializer& objectInitializer);
	FAsyncActionDelegate OnStart;
	FAsyncActionDelegate OnAbort;
	FAsyncActionDelegate OnUpdate;
	FAsyncActionDelegate OnFinishd;

public:
	UFUNCTION(BlueprintCallable, Category = "Action")
		virtual bool OnActionStart(FString& ResultStr);
	UFUNCTION(BlueprintCallable, Category = "Action")
		virtual void OnActionAbort();
	UFUNCTION(BlueprintCallable, Category = "Action")
		virtual void OnActionUpdate();
	UFUNCTION(BlueprintCallable, Category = "Action")
		virtual void OnActionFinishd();

public:
	// Action status (BP Executable function)
	UFUNCTION(BlueprintCallable, Category = "Action")
		virtual bool Action_IsRunning()const;
	UFUNCTION(BlueprintCallable, Category = "Action")
		virtual bool Action_ExecutableStart()const;

protected:
	// End Action
	virtual void EndAction();
	virtual void UnBindAll();

	virtual void InitDelegateList();
	virtual bool Action_IsStarted()const;
	virtual bool Action_EventIsBinded()const;

protected:
	bool mActionStarted = false;
	TArray<FAsyncActionDelegate*> DelegateList;
};

UCLASS(BlueprintType)
class GBWPOWERFULHIT_API UGBWPHAsyncTaskBase : public UBlueprintAsyncActionBase
{
	GENERATED_UCLASS_BODY()

public:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
		static UGBWPHAsyncTaskBase* CreateGBWPHAsyncTask(UGBWPHAsyncActionObject* ActionObj);

public:

	UPROPERTY(BlueprintAssignable)
		FAsyncTaskDelegate OnStart;

	UPROPERTY(BlueprintAssignable)
		FAsyncTaskDelegate OnAbort;

	UPROPERTY(BlueprintAssignable)
		FAsyncTaskDelegate OnUpdate;

	UPROPERTY(BlueprintAssignable)
		FAsyncTaskDelegate OnFinishd;

protected:
	void StartTask(UGBWPHAsyncActionObject* ActionObj);
	virtual void OnActionStart(UGBWPHAsyncActionObject* ActionObj);
	virtual void OnActionAbort(UGBWPHAsyncActionObject* ActionObj);
	virtual void OnActionUpdate(UGBWPHAsyncActionObject* ActionObj);
	virtual void OnActionFinishd(UGBWPHAsyncActionObject* ActionObj);
};


UCLASS(meta = (HideThen = true))
class GBWPOWERFULHIT_API UGBWPHAsync_SetTimer : public UBlueprintAsyncActionBase, public FTickableGameObject
{
	//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTimerHandleDelegate, FTimerHandle, TimerHandle);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FTimerHandleDelegate,
		FTimerHandle, TimerHandle,
		UObject*, ParameterObject,
		float, Time,
		float, DeltaTime,
		UGBWPHAsync_SetTimer*,TimerTask);
	GENERATED_BODY()

public:

	UGBWPHAsync_SetTimer();

	~UGBWPHAsync_SetTimer();

	// FTickableGameObject Begin
	virtual void Tick( float DeltaTime ) override;
	virtual ETickableTickType GetTickableTickType() const override
	{
		return ETickableTickType::Always;
	}
	virtual TStatId GetStatId() const override
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT( FMyTickableThing, STATGROUP_Tickables );
	}
	virtual bool IsTickableWhenPaused() const
	{
		return true;
	}
	virtual bool IsTickableInEditor() const
	{
		return false;
	}
	// FTickableGameObject End


public:

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true",
		WorldContext = "WorldContextObject",
		DisplayName = "SetTimerPro",
		ScriptName = "SetTimerPro",
		AdvancedDisplay = "InitialStartDelay, InitialStartDelayVariance"),
		Category = "GBW|Async")
		static UGBWPHAsync_SetTimer* SetTimer(const UObject* WorldContextObject,
			float Time,
			bool bLooping,
			float InitialStartDelay = 0.f,
			float InitialStartDelayVariance = 0.f,
			UObject* ParameterObject=nullptr);

public: 
	
	UPROPERTY(BlueprintAssignable)
		FTimerHandleDelegate Then;

	UPROPERTY(BlueprintAssignable)
		FTimerHandleDelegate Update;
	
	UPROPERTY(BlueprintAssignable)
		FTimerHandleDelegate Completed;

	UFUNCTION(BlueprintCallable,Category = "SetTimer")
		void Stop();

private:

	virtual void Activate() override;
	
	UFUNCTION()
		void CompletedEvent();
	
	FTimerHandle SetTimerDelegate(FTimerDynamicDelegate Delegate,
		float Time,
		bool bLooping,
		float InitialStartDelay = 0.f,
		float InitialStartDelayVariance = 0.f);
	
	void PreGarbageCollect();

	// The last frame number we were ticked.
	// We don't want to tick multiple times per frame 
	uint32 LastFrameNumberWeTicked = INDEX_NONE;

private:
	const UObject* WorldContextObject;
	FTimerHandle TimerHandle;
	UObject* ParameterObjectCache;
	float Timer = 0.0f;
	float FrameDeltaTime = 0.0f;
	bool bIsCompleted = false;
	
	UWorld* World;
};


UINTERFACE(MinimalAPI)
class UGBWBPI_MontagePlay : public UInterface
{
	GENERATED_BODY()
};
class GBWPOWERFULHIT_API IGBWBPI_MontagePlay
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, Category = "GBW|Anim")
		bool GetRootMotionGravitySetting(bool& InRootMotionIgnoresGravity, float& InGravitationalAcceleration_MS2);
	UFUNCTION(BlueprintNativeEvent, Category = "GBW|Anim")
		bool GetRootMotionScale(float& RootMotionScale);
};
USTRUCT(BlueprintType)
struct FGBWPH_MontagePlayState
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite,Category = "Settings")
	float Time;
	
	UPROPERTY(BlueprintReadWrite,Category = "Settings")
	float MontagePosition;
	
	UPROPERTY(BlueprintReadWrite,Category = "Settings")
	UAnimInstance* AnimInstance;

	UPROPERTY(BlueprintReadWrite,Category = "Settings")
	bool IsMainAnimInstance;
	
	UPROPERTY(BlueprintReadWrite,Category = "Settings")
	USkeletalMeshComponent* SkeletalMeshComponent;
	
	UPROPERTY(BlueprintReadWrite,Category = "Settings")
	UAnimMontage* MontageToPlay;
	
	FGBWPH_MontagePlayState():
	Time(0.0f),
	MontagePosition(0.0f),
	AnimInstance(nullptr),
	IsMainAnimInstance(false),
	SkeletalMeshComponent(nullptr),
	MontageToPlay(nullptr)
	{}
};
USTRUCT(BlueprintType)
struct FGBWPH_RMSInfo_FromMesh
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite,Category = "Settings")
	UCharacterMovementComponent* CharacterMovement = nullptr;
	
	UPROPERTY(BlueprintReadWrite,Category = "Settings")
	ERootMotionAccumulateMode RootMotionAccumulateMode = ERootMotionAccumulateMode::Additive;

	UPROPERTY(BlueprintReadWrite,Category = "Settings")
	FName InstanceName = NAME_None;
	
	UPROPERTY(BlueprintReadWrite,Category = "Settings")
	float StartTime = 0;
	
	UPROPERTY(BlueprintReadWrite,Category = "Settings")
	float EndTime = -1;

	UPROPERTY(BlueprintReadWrite,Category = "Settings")
	float Rate = 1;
	
	UPROPERTY(BlueprintReadWrite,Category = "Settings")
	float RootMotionScale = 1;

	UPROPERTY(BlueprintReadWrite,Category = "Settings")
	bool RootMotionIgnoresGravity = false;
	
	UPROPERTY(BlueprintReadWrite,Category = "Settings")
	float GravitationalAcceleration_ms2 = -9.8f;
	
	FGBWPH_RMSInfo_FromMesh(){}
};
USTRUCT()
struct GBWPOWERFULHIT_API FGBWPHRootMotionSource_FromSKMesh : public FRootMotionSource
{
	GENERATED_USTRUCT_BODY()
	FGBWPHRootMotionSource_FromSKMesh()
	{
	};

	virtual ~FGBWPHRootMotionSource_FromSKMesh()
	{
	}
	
	UPROPERTY()
	UActorComponent* PHAnimInterfaceComponent = nullptr;
	UPROPERTY()
	TObjectPtr<USkeletalMeshComponent> SKMesh = nullptr;
	UPROPERTY()
	FVector StartLocation = FVector::ZeroVector;
	UPROPERTY()
	FRotator StartRotation = FRotator::ZeroRotator;
	UPROPERTY()
	float AnimStartTime = 0;
	UPROPERTY()
	float AnimEndTime = -1;
	UPROPERTY()
	float TheRootMotionScale = 1;
	UPROPERTY()
	bool RootMotionIgnoresGravity = false;
	UPROPERTY()
	float GravitationalAcceleration_ms2 = -9.8f;
protected:
	UPROPERTY()
	FRotator CachedRotation = FRotator::ZeroRotator;
	UPROPERTY()
	FVector CachedTarget = FVector::ZeroVector;
	UPROPERTY()
	float CachedEndTime = -1;

	FORCEINLINE float GetCurrentAnimEndTime() const
	{
		return CachedEndTime <= 0 ? AnimEndTime : CachedEndTime;
	};
	FORCEINLINE FRotator GetTargetRotation() const
	{
		return CachedRotation;
	};
	void SetCurrentAnimEndTime(float NewEndTime)
	{
		CachedEndTime = NewEndTime;
	}
	void SetTargetRotation(FRotator Rotation)
	{
		CachedRotation = Rotation;
	}

	UActorComponent* GetPHAnimInterfaceComponent()
	{
		if (!PHAnimInterfaceComponent)
		{
			if (SKMesh && SKMesh->GetOwner())
			{
				TArray<UActorComponent*> Comps = SKMesh->GetOwner()->GetComponentsByInterface(UGBWBPI_MontagePlay::StaticClass());
                
				if (!Comps.IsEmpty())
				{
					PHAnimInterfaceComponent = Comps[0];
				}
			}
		}

		return PHAnimInterfaceComponent;
	}
	
	void GetGravitySetting(bool& InRootMotionIgnoresGravity, float& InGravitationalAcceleration_ms2)
	{
		if (GetPHAnimInterfaceComponent())
		{
			if (IGBWBPI_MontagePlay::Execute_GetRootMotionGravitySetting(
				GetPHAnimInterfaceComponent(),
				InRootMotionIgnoresGravity,
				InGravitationalAcceleration_ms2))
			{
				return;
			}
		}
		
		InRootMotionIgnoresGravity = RootMotionIgnoresGravity;
		InGravitationalAcceleration_ms2 = GravitationalAcceleration_ms2;
	}
	float GetRootMotionScale()
	{
		if (GetPHAnimInterfaceComponent())
		{
			float res;
			if (IGBWBPI_MontagePlay::Execute_GetRootMotionScale(GetPHAnimInterfaceComponent(),res))
			{
				return res;
			}
		}
		
		return TheRootMotionScale;
	}

public:
	virtual void PrepareRootMotion(
		float SimulationTime,
		float MovementTickTime,
		const ACharacter& Character,
		const UCharacterMovementComponent& MoveComponent
	) override;
	
	virtual bool UpdateStateFrom(const FRootMotionSource* SourceToTakeStateFrom, bool bMarkForSimulatedCatchup = false) override;
	virtual bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess) override;
	virtual FRootMotionSource* Clone() const override;
	virtual bool Matches(const FRootMotionSource* Other) const override;
	virtual bool MatchesAndHasSameState(const FRootMotionSource* Other) const override;
	virtual UScriptStruct* GetScriptStruct() const override;
	virtual FString ToSimpleString() const override;
	virtual void AddReferencedObjects(class FReferenceCollector& Collector) override;
};
template <>
struct TStructOpsTypeTraits<FGBWPHRootMotionSource_FromSKMesh> : public TStructOpsTypeTraitsBase2<FGBWPHRootMotionSource_FromSKMesh>
{
	enum
	{
		WithNetSerializer = true,
		WithCopy = true
	};
};
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnMontagePlayDelegate,
	FName, NotifyName,
	FGBWPH_MontagePlayState, MontagePlayState,
	UObject*, ParameterObject);
UCLASS(MinimalAPI)
class UGBWPlayMontageCallbackProxy : public UObject, public FTickableGameObject
{
	GENERATED_UCLASS_BODY()
	
	// FTickableGameObject Begin
	virtual void Tick( float DeltaTime ) override;
	virtual ETickableTickType GetTickableTickType() const override
	{
		return ETickableTickType::Always;
	}
	virtual TStatId GetStatId() const override
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT( FMyTickableThing, STATGROUP_Tickables );
	}
	virtual bool IsTickableWhenPaused() const
	{
		return true;
	}
	virtual bool IsTickableInEditor() const
	{
		return false;
	}
	// FTickableGameObject End
	
	UPROPERTY(BlueprintAssignable)
	FOnMontagePlayDelegate OnTick;
	
	UPROPERTY(BlueprintAssignable)
	FOnMontagePlayDelegate OnCompleted;

	UPROPERTY(BlueprintAssignable)
	FOnMontagePlayDelegate OnBlendOut;

	UPROPERTY(BlueprintAssignable)
	FOnMontagePlayDelegate OnInterrupted;

	UPROPERTY(BlueprintAssignable)
	FOnMontagePlayDelegate OnNotifyBegin;

	UPROPERTY(BlueprintAssignable)
	FOnMontagePlayDelegate OnNotifyEnd;

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static UGBWPlayMontageCallbackProxy* CreateProxyObjectForPlayMontage(
		class USkeletalMeshComponent* InSkeletalMeshComponent, 
		class UAnimMontage* MontageToPlay, 
		float PlayRate = 1.f, 
		float StartingPosition = 0.f, 
		FName StartingSection = NAME_None,
		bool ApplyRootMotion = true,
		float RootMotionScale = 1.0f,
		ERootMotionAccumulateMode RootMotionAccumulateMode = ERootMotionAccumulateMode::Additive,
		bool RootMotionIgnoresGravity = false,
		float GravitationalAcceleration_ms2 = -9.8f,
		UObject* ParameterObject = nullptr);

public:
	UFUNCTION(BlueprintCallable, Category="GBW|Anim|RMS")
		static bool ApplyRMS_BySKMesh(
			UCharacterMovementComponent* MovementComponent,
			USkeletalMeshComponent* SKMesh,
			ERootMotionAccumulateMode RootMotionAccumulateMode = ERootMotionAccumulateMode::Additive,
			float StartTime = 0,
			float EndTime = -1,
			float Rate = 1,
			float TheRootMotionScale = 1,
			bool RootMotionIgnoresGravity = false,
			float GravitationalAcceleration_ms2 = -9.8f);
	UFUNCTION(BlueprintCallable, Category="GBW|Anim|RMS")
		static bool DisableRMS_BySKMesh(
			UCharacterMovementComponent* MovementComponent,
			USkeletalMeshComponent* SKMesh);
	
	//~ Begin UObject Interface
	virtual void BeginDestroy() override;
	//~ End UObject Interface

protected:
	UFUNCTION()
	void OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnNotifyBeginReceived(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload);

	UFUNCTION()
	void OnNotifyEndReceived(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload);

private:
	bool bIsEnd = false;
	bool bApplyRootMotion = false;
	float TheRootMotionScale = 1.0f;
	
	TWeakObjectPtr<UAnimInstance> AnimInstancePtr;
	ERootMotionMode::Type RootMotionAccumulateModeCache = ERootMotionMode::Type::RootMotionFromMontagesOnly; 
	TWeakObjectPtr<UAnimInstance> MainAnimInstancePtr;
	int32 MontageInstanceID;
	uint32 bInterruptedCalledBeforeBlendingOut : 1;
	
	UObject* TheParameterObject;

	ACharacter* Owner = nullptr;
	
	FGBWPH_MontagePlayState PlayState = FGBWPH_MontagePlayState();
	FGBWPH_RMSInfo_FromMesh RMSInfo = FGBWPH_RMSInfo_FromMesh();

	bool IsNotifyValid(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload) const;
	void UnbindDelegates();

	FOnMontageBlendingOutStarted BlendingOutDelegate;
	FOnMontageEnded MontageEndedDelegate;

	void PlayMontage(
		class USkeletalMeshComponent* InSkeletalMeshComponent,
		class UAnimMontage* MontageToPlay,
		float PlayRate = 1.f,
		float StartingPosition = 0.f,
		FName StartingSection = NAME_None,
		bool ApplyRootMotion = true,
		float RootMotionScale = 1.0f,
		ERootMotionAccumulateMode RootMotionAccumulateMode = ERootMotionAccumulateMode::Additive,
		bool RootMotionIgnoresGravity = false,
		float GravitationalAcceleration_ms2 = -9.8f,
		UObject* ParameterObject = nullptr);

	bool TryActiveRMS();
	bool DisableRMS();
};


// Copyright GanBowen 2022-2023. All Rights Reserved.


#include "GBWPHAsyncTaskBase.h"

#include <string>

#include "Engine/Engine.h"
#include "GBWPowerfulHitFuncLib.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"

UGBWPHAsyncActionObject::UGBWPHAsyncActionObject(const FObjectInitializer& objectInitializer)
{
	InitDelegateList();
}

bool UGBWPHAsyncActionObject::OnActionStart(FString& ResultStr)
{
	ResultStr.Reset();
	bool local_bResault = false;

	if (Action_ExecutableStart())
	{

		mActionStarted = true;
		OnStart.ExecuteIfBound(this);

		local_bResault = true;
	}
	else {
		local_bResault = false;
		if (Action_IsStarted())
		{
			ResultStr.Append(FString(TEXT("Action is Started.\n")));
		}
		if (!Action_EventIsBinded())
		{
			ResultStr.Append(FString(TEXT("Action is not bind to anything Task")));
		}
	}
	return local_bResault;
}

void UGBWPHAsyncActionObject::OnActionAbort()
{
	if (Action_IsRunning())
	{
		OnAbort.ExecuteIfBound(this);
		EndAction();
	}
}

void UGBWPHAsyncActionObject::OnActionUpdate()
{
	if (Action_IsRunning())
	{
		OnUpdate.ExecuteIfBound(this);
	}
}
void UGBWPHAsyncActionObject::OnActionFinishd()
{
	if (Action_IsRunning())
	{
		OnFinishd.ExecuteIfBound(this);
		EndAction();
	}
}

void UGBWPHAsyncActionObject::EndAction()
{
	if (Action_IsRunning())
	{
		UnBindAll();
		mActionStarted = false;
	}
}

void UGBWPHAsyncActionObject::UnBindAll()
{
	for (auto& DeleIndex : DelegateList)
	{
		if (DeleIndex->IsBound())
		{
			DeleIndex->Unbind();
		}
	}
}

void UGBWPHAsyncActionObject::InitDelegateList()
{
	if (!Action_IsStarted())
	{
		DelegateList.AddUnique(&OnStart);
		DelegateList.AddUnique(&OnAbort);
		DelegateList.AddUnique(&OnUpdate);
		DelegateList.AddUnique(&OnFinishd);
	}
}

bool UGBWPHAsyncActionObject::Action_EventIsBinded()const
{
	bool EventIsBinded = true;
	for (auto& DeleIndex : DelegateList)
	{
		if (!DeleIndex->IsBound())
			EventIsBinded = false;
	}
	return EventIsBinded;
}

bool UGBWPHAsyncActionObject::Action_IsStarted()const
{
	return mActionStarted;
}

bool UGBWPHAsyncActionObject::Action_IsRunning()const
{
	return Action_IsStarted() && Action_EventIsBinded();
}

bool UGBWPHAsyncActionObject::Action_ExecutableStart()const
{
	return !Action_IsStarted() && Action_EventIsBinded();
}

UGBWPHAsyncTaskBase::UGBWPHAsyncTaskBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	if (HasAnyFlags(RF_ClassDefaultObject) == false)
	{
		AddToRoot();
	}
}

UGBWPHAsyncTaskBase* UGBWPHAsyncTaskBase::CreateGBWPHAsyncTask(UGBWPHAsyncActionObject* ActionObj)
{
	UGBWPHAsyncTaskBase* AsyncActionTask = NewObject<UGBWPHAsyncTaskBase>();
	AsyncActionTask->StartTask(ActionObj);
	return AsyncActionTask;
}

void UGBWPHAsyncTaskBase::StartTask(UGBWPHAsyncActionObject* ActionObj)
{
	if (ActionObj && !ActionObj->Action_IsRunning())
	{
		(ActionObj->OnStart).BindUObject(this, &UGBWPHAsyncTaskBase::OnActionStart);
		(ActionObj->OnAbort).BindUObject(this, &UGBWPHAsyncTaskBase::OnActionAbort);
		(ActionObj->OnUpdate).BindUObject(this, &UGBWPHAsyncTaskBase::OnActionUpdate);
		(ActionObj->OnFinishd).BindUObject(this, &UGBWPHAsyncTaskBase::OnActionFinishd);
	}
} 

void UGBWPHAsyncTaskBase::OnActionStart(UGBWPHAsyncActionObject* ActionObj)
{
	OnStart.Broadcast(ActionObj);
}
void UGBWPHAsyncTaskBase::OnActionAbort(UGBWPHAsyncActionObject* ActionObj)
{
	OnAbort.Broadcast(ActionObj);
}
void UGBWPHAsyncTaskBase::OnActionUpdate(UGBWPHAsyncActionObject* ActionObj)
{
	OnUpdate.Broadcast(ActionObj);
}
void UGBWPHAsyncTaskBase::OnActionFinishd(UGBWPHAsyncActionObject* ActionObj)
{
	OnFinishd.Broadcast(ActionObj);
}




UGBWPHAsync_SetTimer::UGBWPHAsync_SetTimer()
{
	WorldContextObject = nullptr;
}


UGBWPHAsync_SetTimer::~UGBWPHAsync_SetTimer()
{
	
}

UGBWPHAsync_SetTimer* UGBWPHAsync_SetTimer::SetTimer(const UObject* WorldContextObject,
	float Time,
	bool bLooping,
	float InitialStartDelay /*= 0.f*/,
	float InitialStartDelayVariance /*= 0.f*/,
	UObject* ParameterObject/*= nullptr*/)
{
	if (!WorldContextObject)
	{
		FFrame::KismetExecutionMessage(TEXT("Invalid WorldContextObject. Cannot execute Set Timer."), ELogVerbosity::Error);
		return nullptr;
	}

	InitialStartDelay += FMath::RandRange(-InitialStartDelayVariance, InitialStartDelayVariance);
	if (Time <= 0.f || ((Time + InitialStartDelay) - InitialStartDelayVariance) < 0.f)
	{
		FFrame::KismetExecutionMessage(TEXT("SetTimer passed a negative or zero time.  The associated timer may fail to fire!  If using InitialStartDelayVariance, be sure it is smaller than (Time + InitialStartDelay)."), ELogVerbosity::Warning);
		return nullptr;
	}

	UGBWPHAsync_SetTimer* AsyncNode = NewObject<UGBWPHAsync_SetTimer>();
	AsyncNode->WorldContextObject = WorldContextObject;

	FTimerDynamicDelegate Delegate;
	Delegate.BindUFunction(AsyncNode, FName("CompletedEvent"));
	AsyncNode->TimerHandle = AsyncNode->SetTimerDelegate(Delegate, Time, bLooping, InitialStartDelay, InitialStartDelayVariance);

	AsyncNode->RegisterWithGameInstance((UObject*)WorldContextObject);

	FCoreUObjectDelegates::GetPreGarbageCollectDelegate().AddUObject(AsyncNode, &UGBWPHAsync_SetTimer::PreGarbageCollect);

	AsyncNode->ParameterObjectCache = ParameterObject;
	
	return AsyncNode;
}

FTimerHandle UGBWPHAsync_SetTimer::SetTimerDelegate(FTimerDynamicDelegate Delegate,
	float Time,
	bool bLooping,
	float InitialStartDelay /*= 0.f*/,
	float InitialStartDelayVariance /*= 0.f*/)
{
	FTimerHandle Handle;
	if (Delegate.IsBound())
	{
		World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull);
		if (World)
		{
			InitialStartDelay += FMath::RandRange(-InitialStartDelayVariance, InitialStartDelayVariance);
			if (Time <= 0.f || ((Time + InitialStartDelay) - InitialStartDelayVariance) < 0.f)
			{
				FFrame::KismetExecutionMessage(TEXT("SetTimer passed a negative or zero time.  The associated timer may fail to fire!  If using InitialStartDelayVariance, be sure it is smaller than (Time + InitialStartDelay)."), ELogVerbosity::Warning);
			}

			FTimerManager& TimerManager = World->GetTimerManager();
			Handle = TimerManager.K2_FindDynamicTimerHandle(Delegate);
			TimerManager.SetTimer(Handle, Delegate, Time, bLooping, (Time + InitialStartDelay));
		}
	}
	else
	{
		UE_LOG(LogBlueprintUserMessages, Warning,
			TEXT("SetTimer passed a bad function (%s) or object (%s)"),
			*Delegate.GetFunctionName().ToString(), *GetNameSafe(Delegate.GetUObject()));
	}

	return Handle;
}


void UGBWPHAsync_SetTimer::PreGarbageCollect()
{
	if (bIsCompleted) 
	{
		SetReadyToDestroy();
	}
}

void UGBWPHAsync_SetTimer::Stop()
{
	if(TimerHandle.IsValid())
	{
		FTimerManager& TimerManager = World->GetTimerManager();
    	TimerManager.ClearTimer(TimerHandle);
	}
	CompletedEvent();
}

void UGBWPHAsync_SetTimer::Activate()
{
	if (!WorldContextObject)
	{
		FFrame::KismetExecutionMessage(TEXT("Invalid WorldContextObject. Cannot execute Set Timer."), ELogVerbosity::Error);
		return;
	}

	Then.Broadcast(TimerHandle, ParameterObjectCache,Timer, FrameDeltaTime,this);
}


void UGBWPHAsync_SetTimer::CompletedEvent()
{
	if (bIsCompleted || GWorld->GetWorld()!=World)
	{
		return;
	}
	
	bIsCompleted = true;
	if(ParameterObjectCache)
	{
		if(!IsValid(ParameterObjectCache) || !((UObject*)ParameterObjectCache)->IsValidLowLevel())
		{
			ParameterObjectCache = nullptr;
		}
	}
	
	Completed.Broadcast(TimerHandle, ParameterObjectCache,Timer, FrameDeltaTime,this);
}

void UGBWPHAsync_SetTimer::Tick(float DeltaTime)
{
	if ( LastFrameNumberWeTicked == GFrameCounter )
		return;

	if(bIsCompleted)
		return;

	Timer = Timer + DeltaTime;
	FrameDeltaTime = DeltaTime;
	Update.Broadcast(TimerHandle, ParameterObjectCache,Timer, FrameDeltaTime,this);
	
	LastFrameNumberWeTicked = GFrameCounter;
}

UGBWPlayMontageCallbackProxy::UGBWPlayMontageCallbackProxy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, MontageInstanceID(INDEX_NONE)
	, bInterruptedCalledBeforeBlendingOut(false)
{
}

void FGBWPHRootMotionSource_FromSKMesh::PrepareRootMotion(float SimulationTime, float MovementTickTime,
	const ACharacter& Character, const UCharacterMovementComponent& MoveComponent)
{
	RootMotionParams.Clear();
	
	const FAnimMontageInstance* MontageInstance = (SKMesh && SKMesh->GetAnimInstance()) ? SKMesh->GetAnimInstance()->GetRootMotionMontageInstance() : nullptr;
	
	if (MontageInstance
		&& Duration > SMALL_NUMBER
		&& MovementTickTime > SMALL_NUMBER)
	{
		const float MontagePosition = MontageInstance->GetPosition();
		const float PrevTime = MontagePosition;
		const float CurrTime = (MontagePosition + SimulationTime);
		const FTransform CurrRootMotion = MontageInstance->Montage->ExtractRootMotionFromTrackRange(PrevTime, CurrTime);

		FVector Force = UKismetMathLibrary::TransformDirection(
			SKMesh->GetComponentTransform(),
			CurrRootMotion.GetLocation() * GetRootMotionScale())
		/ MovementTickTime;

		bool IgnoreGravity;
		float Gravity;
		GetGravitySetting(IgnoreGravity,Gravity);
		if (!IgnoreGravity)
		{
			const float FallDistance = MoveComponent.Velocity.Z * MovementTickTime + 0.5f * Gravity * 1000000.0f * MovementTickTime * MovementTickTime;
            Force = Force + FVector(0,0, FallDistance);
		}
		
		const FTransform NewTransform(Force);
		RootMotionParams.Set(NewTransform);
	}

	SetTime(GetTime() + SimulationTime);
}

bool FGBWPHRootMotionSource_FromSKMesh::UpdateStateFrom(const FRootMotionSource* SourceToTakeStateFrom,
	bool bMarkForSimulatedCatchup)
{
	return FRootMotionSource::UpdateStateFrom(SourceToTakeStateFrom, bMarkForSimulatedCatchup);
}

bool FGBWPHRootMotionSource_FromSKMesh::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	if (!FRootMotionSource::NetSerialize(Ar, Map, bOutSuccess))
	{
		return false;
	}
	Ar << StartLocation;
	Ar << StartRotation;
	Ar << AnimStartTime;
	Ar << AnimEndTime;
	Ar << CachedEndTime;
	Ar << SKMesh;
	Ar << TheRootMotionScale;
	Ar << RootMotionIgnoresGravity;
	Ar << GravitationalAcceleration_ms2;

	bOutSuccess = true;
	return true;
}

FRootMotionSource* FGBWPHRootMotionSource_FromSKMesh::Clone() const
{
	FGBWPHRootMotionSource_FromSKMesh* CopyPtr = new FGBWPHRootMotionSource_FromSKMesh(*this);
	return CopyPtr;
}

bool FGBWPHRootMotionSource_FromSKMesh::Matches(const FRootMotionSource* Other) const
{
	if (!FRootMotionSource::Matches(Other))
	{
		return false;
	}
	const FGBWPHRootMotionSource_FromSKMesh* OtherCast = static_cast<const FGBWPHRootMotionSource_FromSKMesh*>(Other);

	return StartLocation == OtherCast->StartLocation
	&& StartRotation == OtherCast->StartRotation
	&& StartTime == OtherCast->StartTime
	&& AnimEndTime == OtherCast->AnimEndTime
	&& SKMesh == OtherCast->SKMesh
	&& TheRootMotionScale == OtherCast->TheRootMotionScale
	&& RootMotionIgnoresGravity == OtherCast->RootMotionIgnoresGravity
	&& GravitationalAcceleration_ms2 == OtherCast->GravitationalAcceleration_ms2;
}

bool FGBWPHRootMotionSource_FromSKMesh::MatchesAndHasSameState(const FRootMotionSource* Other) const
{
	return FRootMotionSource::MatchesAndHasSameState(Other);
}

UScriptStruct* FGBWPHRootMotionSource_FromSKMesh::GetScriptStruct() const
{
	return FGBWPHRootMotionSource_FromSKMesh::StaticStruct();
}

FString FGBWPHRootMotionSource_FromSKMesh::ToSimpleString() const
{
	return FString::Printf(
		TEXT("[ID:%u]FGBWPHRootMotionSource_FromSKMesh %s"), LocalID, *InstanceName.GetPlainNameString());
}

void FGBWPHRootMotionSource_FromSKMesh::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(SKMesh);
	FRootMotionSource::AddReferencedObjects(Collector);
}

void UGBWPlayMontageCallbackProxy::Tick(float DeltaTime)
{
	if(!bIsEnd)
	{
		PlayState.Time+=DeltaTime;
		if (bApplyRootMotion)
		{
			const FAnimMontageInstance* MontageInstance = PlayState.AnimInstance ? PlayState.AnimInstance->GetRootMotionMontageInstance() : nullptr;
			if (MontageInstance)
			{
				PlayState.MontagePosition = MontageInstance->GetPosition();
			}

			TryActiveRMS();
		}
		OnTick.Broadcast(
			NAME_None,
			PlayState,
			TheParameterObject);
	}
}

UGBWPlayMontageCallbackProxy* UGBWPlayMontageCallbackProxy::CreateProxyObjectForPlayMontage(
	class USkeletalMeshComponent* InSkeletalMeshComponent,
	class UAnimMontage* MontageToPlay,
	float PlayRate,
	float StartingPosition,
	FName StartingSection,
	bool ApplyRootMotion,
	float RootMotionScale,
	ERootMotionAccumulateMode RootMotionAccumulateMode,
	bool RootMotionIgnoresGravity,
	float GravitationalAcceleration_ms2,
	UObject* ParameterObject)
{
	UGBWPlayMontageCallbackProxy* Proxy = NewObject<UGBWPlayMontageCallbackProxy>();
	Proxy->SetFlags(RF_StrongRefOnFrame);
	Proxy->PlayMontage(
		InSkeletalMeshComponent,
		MontageToPlay,
		PlayRate,
		StartingPosition,
		StartingSection,
		ApplyRootMotion,
		RootMotionScale,
		RootMotionAccumulateMode,
		RootMotionIgnoresGravity,
		GravitationalAcceleration_ms2,
		ParameterObject);
	return Proxy;
}

bool UGBWPlayMontageCallbackProxy::ApplyRMS_BySKMesh(
	UCharacterMovementComponent* MovementComponent,
	USkeletalMeshComponent* SKMesh,
	ERootMotionAccumulateMode RootMotionAccumulateMode,
	float StartTime,
	float EndTime,
	float Rate,
	float TheRootMotionScale,
	bool RootMotionIgnoresGravity,
	float GravitationalAcceleration_ms2)
{
	const FAnimMontageInstance* MontageInstance = (SKMesh && SKMesh->GetAnimInstance()) ? SKMesh->GetAnimInstance()->GetRootMotionMontageInstance() : nullptr;
	if (!MovementComponent || !MontageInstance)
	{
		return false;
	}

	int32 NewPriority = 0;
	FName InstanceName = SKMesh->GetFName();
	if (auto theRMS = MovementComponent->GetRootMotionSource(InstanceName))
	{
		NewPriority = theRMS->Priority + 1;
		MovementComponent->RemoveRootMotionSource(InstanceName);
	}
	
	const float CurrEndTime = (EndTime < 0 || EndTime > MontageInstance->Montage->GetPlayLength())
								  ? MontageInstance->Montage->GetPlayLength()
								  : EndTime;
	const float Duration = (CurrEndTime - StartTime) / FMath::Max(Rate * MontageInstance->Montage->RateScale, 0.1f);
	TSharedPtr<FGBWPHRootMotionSource_FromSKMesh> RMS = MakeShared<FGBWPHRootMotionSource_FromSKMesh>();
	RMS->InstanceName = InstanceName == NAME_None ? TEXT("GBWRMS") : InstanceName;
	RMS->AccumulateMode = RootMotionAccumulateMode;
	RMS->Priority = NewPriority;
	RMS->StartLocation = MovementComponent->GetOwner()->GetActorLocation();
	RMS->StartRotation = MovementComponent->GetOwner()->GetActorRotation();
	RMS->Duration = Duration;
	RMS->AnimStartTime = StartTime;
	RMS->AnimEndTime = EndTime;
	RMS->SKMesh = SKMesh;
	RMS->TheRootMotionScale = TheRootMotionScale;
	RMS->RootMotionIgnoresGravity = RootMotionIgnoresGravity;
	RMS->GravitationalAcceleration_ms2 = GravitationalAcceleration_ms2;
	RMS->SetTime(StartTime);
	return MovementComponent->ApplyRootMotionSource(RMS) > -1;
}

bool UGBWPlayMontageCallbackProxy::DisableRMS_BySKMesh(
	UCharacterMovementComponent* MovementComponent,
	USkeletalMeshComponent* SKMesh)
{
	const FAnimMontageInstance* MontageInstance = (SKMesh && SKMesh->GetAnimInstance()) ? SKMesh->GetAnimInstance()->GetRootMotionMontageInstance() : nullptr;
	if (!MovementComponent || !MontageInstance)
	{
		return false;
	}
	
	FName InstanceName = SKMesh->GetFName();
	MovementComponent->RemoveRootMotionSource(InstanceName);

	return true;
}

void UGBWPlayMontageCallbackProxy::PlayMontage(
	class USkeletalMeshComponent* InSkeletalMeshComponent, 
	class UAnimMontage* MontageToPlay, 
	float PlayRate, 
	float StartingPosition, 
	FName StartingSection,
	bool ApplyRootMotion,
	float RootMotionScale,
	ERootMotionAccumulateMode RootMotionAccumulateMode,
	bool RootMotionIgnoresGravity,
	float GravitationalAcceleration_ms2,
	UObject* ParameterObject)
{
	bool bPlayedSuccessfully = false;
	if (InSkeletalMeshComponent)
	{
		if (UAnimInstance* AnimInstance = InSkeletalMeshComponent->GetAnimInstance())
		{
			const float MontageLength = AnimInstance->Montage_Play(MontageToPlay, PlayRate, EMontagePlayReturnType::MontageLength, StartingPosition);
			bPlayedSuccessfully = (MontageLength > 0.f);

			if (bPlayedSuccessfully)
			{
				AnimInstancePtr = AnimInstance;
				PlayState.AnimInstance = AnimInstancePtr.Get();
				PlayState.SkeletalMeshComponent = InSkeletalMeshComponent;
				PlayState.MontageToPlay = MontageToPlay;
				TheParameterObject = ParameterObject;
				bApplyRootMotion = ApplyRootMotion;
				if (bApplyRootMotion && !MontageToPlay->HasRootMotion())
				{
					bApplyRootMotion = false;
				}
				TheRootMotionScale = RootMotionScale;
				if (Cast<ACharacter>(InSkeletalMeshComponent->GetOwner()))
				{
					MainAnimInstancePtr = Cast<ACharacter>(InSkeletalMeshComponent->GetOwner())->GetMesh()->GetAnimInstance();
				}
				PlayState.IsMainAnimInstance = (MainAnimInstancePtr == AnimInstancePtr);
				
				if (FAnimMontageInstance* MontageInstance = AnimInstance->GetActiveInstanceForMontage(MontageToPlay))
				{
					MontageInstanceID = MontageInstance->GetInstanceID();
				}

				if (StartingSection != NAME_None)
				{		
					AnimInstance->Montage_JumpToSection(StartingSection, MontageToPlay);
				}		

				BlendingOutDelegate.BindUObject(this, &UGBWPlayMontageCallbackProxy::OnMontageBlendingOut);
				AnimInstance->Montage_SetBlendingOutDelegate(BlendingOutDelegate, MontageToPlay);

				MontageEndedDelegate.BindUObject(this, &UGBWPlayMontageCallbackProxy::OnMontageEnded);
				AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, MontageToPlay);

				AnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &UGBWPlayMontageCallbackProxy::OnNotifyBeginReceived);
				AnimInstance->OnPlayMontageNotifyEnd.AddDynamic(this, &UGBWPlayMontageCallbackProxy::OnNotifyEndReceived);
				
				RMSInfo.RootMotionAccumulateMode = RootMotionAccumulateMode;
				RMSInfo.InstanceName = PlayState.SkeletalMeshComponent->GetFName();
				RMSInfo.StartTime = StartingPosition;
				RMSInfo.EndTime = MontageLength;
				RMSInfo.Rate = PlayRate;
				RMSInfo.RootMotionScale = TheRootMotionScale;
				RMSInfo.RootMotionIgnoresGravity = RootMotionIgnoresGravity;
				RMSInfo.GravitationalAcceleration_ms2 = GravitationalAcceleration_ms2;
				TryActiveRMS();
			}
		}
	}

	if (!bPlayedSuccessfully)
	{
		OnInterrupted.Broadcast(
			NAME_None,
			PlayState,
			TheParameterObject);
	}
}

bool UGBWPlayMontageCallbackProxy::TryActiveRMS()
{
	if (bApplyRootMotion)
	{
		if (!RMSInfo.CharacterMovement)
		{
			if (!Owner)
			{
				Owner = Cast<ACharacter>(PlayState.SkeletalMeshComponent->GetOwner());
			}
			RMSInfo.CharacterMovement = Owner->GetCharacterMovement();
		}
		if (!RMSInfo.CharacterMovement->GetRootMotionSource(RMSInfo.InstanceName))
		{
			if( ApplyRMS_BySKMesh(
				RMSInfo.CharacterMovement,
				PlayState.SkeletalMeshComponent,
				RMSInfo.RootMotionAccumulateMode,
				RMSInfo.StartTime,
				RMSInfo.EndTime,
				RMSInfo.Rate,
				RMSInfo.RootMotionScale,
				RMSInfo.RootMotionIgnoresGravity,
				RMSInfo.GravitationalAcceleration_ms2))
			{
				RootMotionAccumulateModeCache = AnimInstancePtr->RootMotionMode;
				AnimInstancePtr->SetRootMotionMode(ERootMotionMode::Type::IgnoreRootMotion);
				return true;
			}
		}
	}
	return false;
}

bool UGBWPlayMontageCallbackProxy::DisableRMS()
{
	if (bApplyRootMotion)
	{
		if (!RMSInfo.CharacterMovement)
		{
			if (!Owner)
			{
				Owner = Cast<ACharacter>(PlayState.SkeletalMeshComponent->GetOwner());
			}
			RMSInfo.CharacterMovement = Owner->GetCharacterMovement();
		}
		if (RMSInfo.CharacterMovement->GetRootMotionSource(RMSInfo.InstanceName))
		{
			if(DisableRMS_BySKMesh(
				RMSInfo.CharacterMovement,
				PlayState.SkeletalMeshComponent))
			{
				AnimInstancePtr->SetRootMotionMode(RootMotionAccumulateModeCache);
				return true;
			}
		}
	}
	return false;
}

bool UGBWPlayMontageCallbackProxy::IsNotifyValid(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload) const
{
	return ((MontageInstanceID != INDEX_NONE) && (BranchingPointNotifyPayload.MontageInstanceID == MontageInstanceID));
}


void UGBWPlayMontageCallbackProxy::OnNotifyBeginReceived(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload)
{
	if (IsNotifyValid(NotifyName, BranchingPointNotifyPayload))
	{
		OnNotifyBegin.Broadcast(
			NotifyName,
			PlayState,
			TheParameterObject);
	}
}


void UGBWPlayMontageCallbackProxy::OnNotifyEndReceived(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload)
{
	if (IsNotifyValid(NotifyName, BranchingPointNotifyPayload))
	{
		OnNotifyEnd.Broadcast(
			NotifyName,
			PlayState,
			TheParameterObject);
	}
}


void UGBWPlayMontageCallbackProxy::OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted)
{
	if (bInterrupted)
	{
		OnInterrupted.Broadcast(
			NAME_None,
			PlayState,
			TheParameterObject);
		bInterruptedCalledBeforeBlendingOut = true;
	}
	else
	{
		OnBlendOut.Broadcast(
			NAME_None,
			PlayState,
			TheParameterObject);
		DisableRMS();
	}
}

void UGBWPlayMontageCallbackProxy::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (!bInterrupted)
	{
		OnCompleted.Broadcast(
			NAME_None,
			PlayState,
			TheParameterObject);
	}
	else if (!bInterruptedCalledBeforeBlendingOut)
	{
		OnInterrupted.Broadcast(
			NAME_None,
			PlayState,
			TheParameterObject);
	}

	UnbindDelegates();
}

void UGBWPlayMontageCallbackProxy::UnbindDelegates()
{
	bIsEnd = true;
	if (UAnimInstance* AnimInstance = AnimInstancePtr.Get())
	{
		DisableRMS();
	
		AnimInstance->OnPlayMontageNotifyBegin.RemoveDynamic(this, &UGBWPlayMontageCallbackProxy::OnNotifyBeginReceived);
		AnimInstance->OnPlayMontageNotifyEnd.RemoveDynamic(this, &UGBWPlayMontageCallbackProxy::OnNotifyEndReceived);
	}
}

void UGBWPlayMontageCallbackProxy::BeginDestroy()
{
	UnbindDelegates();

	Super::BeginDestroy();
}
// Copyright 2022 Mickael Daniel. All Rights Reserved.


#include "Graph/ComboGraphEdge.h"

#include "ComboGraphLog.h"
#include "InputAction.h"
#include "Abilities/ComboGraphNativeTags.h"
#include "Components/ComboGraphGameplayTasksComponent.h"
#include "Net/UnrealNetwork.h"

void UComboGraphEdge::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UComboGraphEdge, CurrentInputActionInstance);
}

ETriggerEvent UComboGraphEdge::GetEnhancedInputTriggerEvent() const
{
	// Convert out internal enum to the real Input Trigger Event for Enhanced Input
	return TriggerEvent == EComboGraphTransitionInputEvent::Started ? ETriggerEvent::Started
		: TriggerEvent == EComboGraphTransitionInputEvent::Triggered ? ETriggerEvent::Triggered
		: TriggerEvent == EComboGraphTransitionInputEvent::Canceled ? ETriggerEvent::Canceled
		: ETriggerEvent::Triggered;
}

void UComboGraphEdge::ReceivedInputConfirm(const FInputActionInstance& InputActionInstance)
{
	if (!OwningActor)
	{
		CG_RUNTIME_LOG(Error, TEXT("UComboGraphEdge::ReceivedInputConfirm %s with InputAction: %s but owning actor is not set"), *GetName(), *GetNameSafe(TransitionInput))
		return;
	}

	if (!OwningTask)
	{
		CG_RUNTIME_LOG(Error, TEXT("UComboGraphEdge::ReceivedInputConfirm %s with InputAction: %s but owning task is not set"), *GetName(), *GetNameSafe(TransitionInput))
		return;
	}

	// Only consider if combo window is currently active / opened (except cancel trigger event, we always want to transition regardless of combo window)
	if (!IsUsingCanceledTriggerEvent() && !OwningTask->IsComboWindowOpened())
	{
		return;
	}

	// Bookkeeping triggers as we only want to transition once, and default behavior with no triggers (or just the down one) will trigger every frame
	if (bConfirmed)
	{
		return;
	}

	bConfirmed = true;
	CurrentInputActionInstance = InputActionInstance;

	const FGameplayTag Tag = FComboGraphNativeTags::Get().Input;
	FGameplayEventData EventData;
	EventData.OptionalObject = TransitionInput;

	// We only need the trigger event type, consider using either instigator / target tags to hold this info, or even EventTag.
	EventData.OptionalObject2 = this;

	CG_RUNTIME_LOG(
		Verbose,
		TEXT("UComboGraphEdge::ReceivedInputConfirm %s (%d) - %s for %s.\nSend Event %s. Behavior: %s, TriggerEvent: %s"),
		*GetName(),
		GetUniqueID(),
		*GetNameSafe(TransitionInput),
		*GetNameSafe(OwningActor),
		*Tag.ToString(),
		*UEnum::GetValueAsString(TransitionBehavior),
		*UEnum::GetValueAsString(TriggerEvent)
	)

	// TODO: Figure out if it can be done via Task (replicated send gameplay event)
	UComboGraphGameplayTasksComponent* Component = OwningActor->FindComponentByClass<UComboGraphGameplayTasksComponent>();
	if (Component)
	{
		Component->SendGameplayEventReplicated(Tag, EventData);
	}
}

void UComboGraphEdge::SetOwningTask(UComboGraphAbilityTask_StartGraph* InOwningTask)
{
	OwningTask = InOwningTask;
}

#if WITH_EDITOR
void UComboGraphEdge::SetNodeTitle(const FText& InTitle)
{
	NodeTitle = InTitle;
}
#endif

bool UComboGraphEdge::IsUsingCanceledTriggerEvent() const
{
	return GetEnhancedInputTriggerEvent() == ETriggerEvent::Canceled;
}

// Copyright 2022 Mickael Daniel. All Rights Reserved.

#include "Animations/GSCNativeAnimInstance.h"

#include "AbilitySystemGlobals.h"

#if WITH_EDITOR
EDataValidationResult UGSCNativeAnimInstance::IsDataValid(TArray<FText>& ValidationErrors)
{
	const EDataValidationResult SuperResult = Super::IsDataValid(ValidationErrors);
	const EDataValidationResult Result = GameplayTagPropertyMap.IsDataValid(this, ValidationErrors);
	return CombineDataValidationResults(SuperResult, Result);
}
#endif

void UGSCNativeAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwningActor()))
	{
		InitializeWithAbilitySystem(ASC);
	}
}
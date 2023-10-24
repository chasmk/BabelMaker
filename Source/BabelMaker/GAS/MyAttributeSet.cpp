// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectExtension.h"



float UMyAttributeSet::GetHealth() const
{
	//保证血量不低于0
	return FMath::Max(Health.GetCurrentValue(), 0.0f);
}

void UMyAttributeSet::SetHealth(float NewVal)
{
	//保证血量不低于0
	NewVal = FMath::Max(NewVal, 0.0f);

	// 获取我们的ASC实例
	UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();
	if (ensure(ASC))
	{
		// Set the base value (not the current value) through the appropriate function.
		// This makes sure that any modifiers we have applied will still work properly.
		ASC->SetNumericAttributeBase(GetHealthAttribute(), NewVal);
	}
}

void UMyAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	// Check to see if this call affects our Health by using the Property Getter.
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		// This Gameplay Effect is changing Health. Apply it, but restrict the value first.
		// In this case, Health's base value must be non-negative.
		SetHealth(FMath::Max(GetHealth(), 0.0f));
	}
}
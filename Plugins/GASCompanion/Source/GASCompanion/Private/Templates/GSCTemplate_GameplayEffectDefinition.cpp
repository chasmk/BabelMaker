// Copyright 2020 Mickael Daniel. All Rights Reserved.


#include "Templates/GSCTemplate_GameplayEffectDefinition.h"

#include "GSCLog.h"

UGSCTemplate_GameplayEffectDefinition::UGSCTemplate_GameplayEffectDefinition()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;
	bExecutePeriodicEffectOnApplication = true;
	PeriodicInhibitionPolicy = EGameplayEffectPeriodInhibitionRemovedPolicy::NeverReset;
	ChanceToApplyToTarget.SetValue(1.f);
	StackingType = EGameplayEffectStackingType::None;
	StackLimitCount = 0;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
	bRequireModifierSuccessToTriggerCues = true;
}

void UGSCTemplate_GameplayEffectDefinition::PostInitProperties()
{
	UObject::PostInitProperties();

	InheritableGameplayEffectTags.PostInitProperties();
	InheritableOwnedTagsContainer.PostInitProperties();
	RemoveGameplayEffectsWithTags.PostInitProperties();
}

#if WITH_EDITOR
void UGSCTemplate_GameplayEffectDefinition::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	UObject::PostEditChangeProperty(PropertyChangedEvent);

	const FProperty* PropertyThatChanged = PropertyChangedEvent.MemberProperty;
	if (PropertyThatChanged)
	{
		const UGameplayEffect* Parent = Cast<UGameplayEffect>(GetClass()->GetSuperClass()->GetDefaultObject());
		const FName PropName = PropertyThatChanged->GetFName();
		if (PropName == GET_MEMBER_NAME_CHECKED(UGameplayEffect, InheritableGameplayEffectTags))
		{
			InheritableGameplayEffectTags.UpdateInheritedTagProperties(Parent ? &Parent->InheritableGameplayEffectTags : NULL);
		}
		else if (PropName == GET_MEMBER_NAME_CHECKED(UGameplayEffect, InheritableOwnedTagsContainer))
		{
			InheritableOwnedTagsContainer.UpdateInheritedTagProperties(Parent ? &Parent->InheritableOwnedTagsContainer : NULL);
		}
		else if (PropName == GET_MEMBER_NAME_CHECKED(UGameplayEffect, RemoveGameplayEffectsWithTags))
		{
			RemoveGameplayEffectsWithTags.UpdateInheritedTagProperties(Parent ? &Parent->RemoveGameplayEffectsWithTags : NULL);
		}
	}
}
#endif

void UGSCTemplate_GameplayEffectDefinition::CopyProperties(UGameplayEffect* CDO, UGSCTemplate_GameplayEffectDefinition* TemplateCDO)
{
	check(CDO);
	check(TemplateCDO);

	GSC_LOG(Verbose, TEXT("UGSCTemplate_GameplayEffectDefinition::CopyProperties for %s from %s"), *GetNameSafe(CDO), *GetNameSafe(TemplateCDO))
	UEngine::CopyPropertiesForUnrelatedObjects(TemplateCDO, CDO);
}

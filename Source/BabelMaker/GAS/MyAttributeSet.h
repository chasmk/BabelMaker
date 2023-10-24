// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "MyAttributeSet.generated.h"

/**
 * 
 */
UCLASS()
class BABELMAKER_API UMyAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
public:
	/** 属性集 Attribute Set. */
	UPROPERTY()
	const UAttributeSet* AttributeSet;
	
	/** 血量属性 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	FGameplayAttributeData Health;
	FGameplayAttribute aa;
	/** 耐力属性 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	FGameplayAttributeData Stamina;
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;

private:

public:
	//~ Helper functions for "Health" attributes
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(UMyAttributeSet, Health);
	float GetHealth() const;
	void SetHealth(float NewVal);
	GAMEPLAYATTRIBUTE_VALUE_INITTER(Health);

};

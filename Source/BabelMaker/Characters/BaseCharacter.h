// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "BabelMaker/GAS/MyAttributeSet.h"
#include "BaseCharacter.generated.h"

UCLASS()
class BABELMAKER_API ABaseCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ABaseCharacter();
	
	
	
	/** 能力系统组件，每个类只能有一个. Required to use Gameplay Attributes and Gameplay Abilities. */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	UAbilitySystemComponent* AbilitySystemComponent;


	/** Sample Attribute Set. */
	UPROPERTY()
	const UMyAttributeSet* AttributeSet;
	
protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
};

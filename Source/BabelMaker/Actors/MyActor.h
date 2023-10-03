// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Actor.h"
#include "MyActor.generated.h"

UCLASS()
class BABELMAKER_API AMyActor : public AActor, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:	
	AMyActor();

	//~ Begin IAbilitySystemInterface
	/** Returns our Ability System Component. */
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	//~ End IAbilitySystemInterface
	
	/** Ability System Component. Required to use Gameplay Attributes and Gameplay Abilities. */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	UAbilitySystemComponent* AbilitySystemComponent;

protected:
	virtual void BeginPlay() override;

private:
	

public:	
	virtual void Tick(float DeltaTime) override;

};

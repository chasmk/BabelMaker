// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"
#include "AbilitySystemComponent.h"

// Sets default values
ABaseCharacter::ABaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	
}

UAbilitySystemComponent* ABaseCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
	
}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	// 初始化AttributeSet   Get the UMyAttributeSet from our Ability System Component. The Ability System Component will create and register one if needed.
	AttributeSet = GetAbilitySystemComponent()->GetSet<UMyAttributeSet>();
	// We now have a pointer to the new UMyAttributeSet that we can use later. If it has an initialization function, this is a good place to call it.


	//AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetHealthAttribute()).AddUObject(this, &BaseCharacter::OnHealthChangedInternal);
}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


// Copyright GanBowen 2022-2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "K2Node_BaseAsyncTask.h"
#include "UK2Node_GBWPlayMontage.generated.h"

/**
 * 
 */
UCLASS()
class GBWPOWERFULHITEDITOR_API UK2Node_GBWPlayMontage : public UK2Node_BaseAsyncTask
{
	GENERATED_UCLASS_BODY()

	//~ Begin UEdGraphNode Interface
	virtual FText GetTooltipText() const override
	{
		return FText::FromString("Plays a Montage on a SkeletalMeshComponent, And return relevant information of montage in the callback");
	}
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override
	{
		return FText::FromString("GBWPlayMontage");
	}
	virtual void GetPinHoverText(const UEdGraphPin& Pin, FString& HoverTextOut) const override;
	//~ End UEdGraphNode Interface

	//~ Begin UK2Node Interface
	virtual FText GetMenuCategory() const override
	{
		return FText::FromString("GBW|PowerfulHit|Animation");	
	}
	//~ End UK2Node Interface
	
};

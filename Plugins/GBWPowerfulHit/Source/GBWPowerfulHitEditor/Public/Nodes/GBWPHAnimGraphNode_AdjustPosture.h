// Copyright GanBowen 2022-2023. All Rights Reserved.

#pragma once

#include "AnimGraphNode_SkeletalControlBase.h"
#include "AnimNodes/GBWPHAnimNode_PostureAdjust.h"
#include "EdGraph/EdGraphNodeUtils.h"
#include "GBWPHAnimGraphNode_AdjustPosture.generated.h"

class FCompilerResultsLog;

UCLASS()
class GBWPOWERFULHITEDITOR_API UGBWPHAnimGraphNode_PostureAdjust : public UAnimGraphNode_SkeletalControlBase
{
	GENERATED_BODY()

	UGBWPHAnimGraphNode_PostureAdjust(const FObjectInitializer& ObjectInitializer);
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	FGBWPHAnimNode_PostureAdjust Node;

public:
	// UEdGraphNode interface
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FText GetTooltipText() const override;
	// End of UEdGraphNode interface

	protected:
	// UAnimGraphNode_Base interface
	virtual void ValidateAnimNodeDuringCompilation(USkeleton* ForSkeleton, FCompilerResultsLog& MessageLog) override;
	// End of UAnimGraphNode_Base interface

	// UAnimGraphNode_SkeletalControlBase interface
	virtual FText GetControllerDescription() const override;
	virtual const FAnimNode_SkeletalControlBase* GetNode() const override { return &Node; }
	// End of UAnimGraphNode_SkeletalControlBase interface

private:
	/** Constructing FText strings can be costly, so we cache the node's title */
	FNodeTitleTextTable CachedNodeTitles;
};

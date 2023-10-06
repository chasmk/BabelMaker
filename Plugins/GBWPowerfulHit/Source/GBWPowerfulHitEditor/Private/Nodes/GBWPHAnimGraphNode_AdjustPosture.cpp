// Copyright GanBowen 2022-2023. All Rights Reserved.

#include "Nodes/GBWPHAnimGraphNode_AdjustPosture.h"
#include "AnimNodeEditModes.h"
#include "Animation/AnimNode_Inertialization.h"
#include "Kismet2/CompilerResultsLog.h"

#define LOCTEXT_NAMESPACE "GBWPHPostureAdjustAnimationGraphNode"

UGBWPHAnimGraphNode_PostureAdjust::UGBWPHAnimGraphNode_PostureAdjust(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UGBWPHAnimGraphNode_PostureAdjust::ValidateAnimNodeDuringCompilation(USkeleton* ForSkeleton, FCompilerResultsLog& MessageLog)
{
	Super::ValidateAnimNodeDuringCompilation(ForSkeleton, MessageLog);
}

FText UGBWPHAnimGraphNode_PostureAdjust::GetControllerDescription() const
{
	return LOCTEXT("PH_PostureAdjust", "PH_PostureAdjust");
}

FText UGBWPHAnimGraphNode_PostureAdjust::GetTooltipText() const
{
	return LOCTEXT("PH_PostureAdjust_Tooltip", "PH_PostureAdjust");
}

FText UGBWPHAnimGraphNode_PostureAdjust::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("PH_PostureAdjust_Tooltip", "PH_PostureAdjust");
}
#undef LOCTEXT_NAMESPACE

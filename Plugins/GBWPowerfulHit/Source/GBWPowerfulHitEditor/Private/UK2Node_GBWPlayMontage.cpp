// Copyright GanBowen 2022-2023. All Rights Reserved.


#include "UK2Node_GBWPlayMontage.h"

#include "GBWPHAsyncTaskBase.h"

UK2Node_GBWPlayMontage::UK2Node_GBWPlayMontage(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ProxyFactoryFunctionName = GET_FUNCTION_NAME_CHECKED(UGBWPlayMontageCallbackProxy, CreateProxyObjectForPlayMontage);
	ProxyFactoryClass = UGBWPlayMontageCallbackProxy::StaticClass();
	ProxyClass = UGBWPlayMontageCallbackProxy::StaticClass();
}

void UK2Node_GBWPlayMontage::GetPinHoverText(const UEdGraphPin& Pin, FString& HoverTextOut) const
{
	Super::GetPinHoverText(Pin, HoverTextOut);
	
	static const FName NAME_OnNotifyBegin = FName(TEXT("OnNotifyBegin"));
	static const FName NAME_OnNotifyEnd = FName(TEXT("OnNotifyEnd"));

	if (Pin.PinName == NAME_OnNotifyBegin)
	{
		FText ToolTipText = FText::FromString("Event called when using a PlayMontageNotify or PlayMontageNotifyWindow Notify in a Montage.");
		HoverTextOut = FString::Printf(TEXT("%s\n%s"), *ToolTipText.ToString(), *HoverTextOut);
	}
	else if (Pin.PinName == NAME_OnNotifyEnd)
	{
		FText ToolTipText = FText::FromString("Event called when using a PlayMontageNotifyWindow Notify in a Montage.");
		HoverTextOut = FString::Printf(TEXT("%s\n%s"), *ToolTipText.ToString(), *HoverTextOut);
	}
}
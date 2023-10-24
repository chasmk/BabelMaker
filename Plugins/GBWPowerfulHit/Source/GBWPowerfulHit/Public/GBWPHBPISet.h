// Copyright GanBowen 2022-2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GBWPHBPISet.generated.h"

USTRUCT(BlueprintType)
struct FGBWPH_Anim_PostureAdjuestData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Settings")
	FName PostureRootBoneName;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Settings")
	FVector TargetPoint;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Settings")
	FVector2D RollAdjustRange;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Settings")
	FVector2D YawAdjustRange;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Settings")
    float BaseRollAdjust;
    
    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Settings")
    float BaseYawAdjust;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Settings")
	float AdjustRate;
	
	FGBWPH_Anim_PostureAdjuestData():
	PostureRootBoneName(NAME_None),
	TargetPoint(FVector::Zero()),
	RollAdjustRange(FVector2D(-70.0f,70.0f)),
	YawAdjustRange(FVector2D(-70.0f,70.0f)),
	BaseRollAdjust(0.0f),
	BaseYawAdjust(0.0f),
	AdjustRate(1.0f)
	{
	}
};

USTRUCT(BlueprintType)
struct FGBWPH_Anim_BodyShakeData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Settings")
	FName BoneName;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Settings")
	float Degree;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Settings")
	float Duration;
	
	FGBWPH_Anim_BodyShakeData():
	BoneName(NAME_None),
	Degree(2.0f),
	Duration(0.2f)
	{
	}
};

USTRUCT(BlueprintType)
struct FGBWPH_Anim_BodyTwistData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Settings")
	FName BoneName;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Settings")
	FRotator Rotation;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Settings")
	float InTime;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Settings")
	float HoldTime;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Settings")
	float OutTime;
	
	FGBWPH_Anim_BodyTwistData():
	BoneName(NAME_None),
	Rotation(FRotator::ZeroRotator),
	InTime(0.1f),
	HoldTime(0.2f),
	OutTime(0.1f)
	{
	}

	float GetTotalTime()
	{
		return InTime+HoldTime+OutTime;
	}

	bool isIn(float time)
	{
		return time<InTime;
	}
	bool isHold(float time)
	{
		return time>=InTime && time<InTime+HoldTime;
	}
	bool isOut(float time)
	{
		return time>=InTime+HoldTime && time<GetTotalTime();
	}
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UGBWPHBPI_Anim : public UInterface
{
	GENERATED_BODY()
};

class GBWPOWERFULHIT_API IGBWPHBPI_Anim
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, Category = "GBW|PowerfulHit|Anim")
    	bool GetPostureAdjustData(FGBWPH_Anim_PostureAdjuestData& PostureAdjuestData);

	UFUNCTION(BlueprintNativeEvent, Category = "GBW|PowerfulHit|Anim")
		bool GetBodyShakeData(bool ClearData, FGBWPH_Anim_BodyShakeData& BodyShakeData);

	UFUNCTION(BlueprintNativeEvent, Category = "GBW|PowerfulHit|Anim")
		bool GetBodyTwistData(bool ClearData, FGBWPH_Anim_BodyTwistData& BodyTwistData);
};

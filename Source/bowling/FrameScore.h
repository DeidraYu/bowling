// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "FrameScore.generated.h"

/**
 * 
 */
UCLASS()
class BOWLING_API UFrameScore : public UObject
{
	GENERATED_BODY()
public:
	int32 Roll1Score = 0;
	int32 Roll2Score = 0;
	FString Roll1String = TEXT(" ");
	FString Roll2String = TEXT(" ");
};

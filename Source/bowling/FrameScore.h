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

private:
	int32 Score = 0;


public:
	int32 Roll1Score = 0;
	int32 Roll2Score = 0;
	FString Roll1String = TEXT(" ");
	FString Roll2String = TEXT(" ");

	bool FrameFinished = false;
	bool FrameHasScore = false;

	// TArray<int32>* RollScores = nullptr;
	// void SetRollScores(TArray<int32>* InRollScores);

	// Index of the roll in the RollScores array that the first roll in the frame corresponds to.
	int32 RollIndex = -1;

	bool IsSpare();
	bool IsStrike();
	bool IsOpen();

	bool IsFinished() const { return FrameFinished; }

	int32 GetFrameScore() const;
	void SetFrameScore(int32 NewScore);


	void PrintFrameScore() const {

		UE_LOG(LogTemp, Display, TEXT("RollIndex:     %d"), RollIndex);
		UE_LOG(LogTemp, Display, TEXT("Roll1Score:    %d"), Roll1Score);
		UE_LOG(LogTemp, Display, TEXT("Roll2Score:    %d"), Roll2Score);
		UE_LOG(LogTemp, Display, TEXT("Roll2String:   %s"), *Roll2String);

		UE_LOG(LogTemp, Display, TEXT("FrameFinished: %d"), FrameFinished);
		UE_LOG(LogTemp, Display, TEXT("FrameHasScore: %d"), FrameHasScore);
		UE_LOG(LogTemp, Display, TEXT("Score:         %d"), Score);


	}
};

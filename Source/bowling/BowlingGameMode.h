// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet/GameplayStatics.h"

#include "Ball.h"
#include "Pin.h"

#include "ScreenMessage.h"
#include "FrameScore.h"

#include "BowlingGameMode.generated.h"

/**
 * 
 */
UCLASS()
class BOWLING_API ABowlingGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ABowlingGameMode();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(EditAnywhere) TSubclassOf<UScreenMessage> ScreenMessageClass;
	UScreenMessage* ScreenMessageWidget;

	ABall* Ball;
	int32 PinCount = 10;
	TArray<APin*> Pins;

	float GameOverDelay = 3.0f;
	bool IsThrowFinished();

	void TriggerRollOver();
	void OnRollOverTimerTimeout();

	void TriggerFrameOver();
	void OnFrameOverTimerTimeout();


	void TriggerGameOver();
	void OnGameOverTimerTimeout();
		
	int32 RollFinished();


	UPROPERTY(EditAnywhere) int32 NumFrames = 10;

	void Roll(int32 RollScore);
	int32 Score();
	void DrawScores();
	void ComputeScoresPass1();
	int32 ComputeScoresPass2();


	// The frames are numbered 0, 1, ..., NumFrames-1
	int32 FrameNumber = 0;
	
	// For each frame maintain the number of pins that have been scored sofar
	int32 PinsInFrame = 0;

	// For each frame, count the number of rolls we have been doing sofar
	// The rolls in a typical frame are 0 and 1
	// For a strike there is only a roll 0
	// For a spare the rolls are 0 and 1
	
	int32 RollNumberInFrame = 0;
	bool HaveThirdRoll = false;
	TArray<int32> RollScores;

	UPROPERTY()
	TArray<UFrameScore*> FrameScores;
	FString CreateSeparatorSting(FString LeftSep, FString InnerSep, FString RightSep) const;
	FString CreateFrameNumbersString() const;
	FString CreateRollScoresString() const;
	FString CreateFrameScoresString() const;
	
};

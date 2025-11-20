// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "Ball.h"
#include "Pin.h"

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
	ABall* Ball;
	int32 PinCount = 10;
	TArray<APin*> Pins;

	bool IsThrowFinished();
};

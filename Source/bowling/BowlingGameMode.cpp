// Fill out your copyright notice in the Description page of Project Settings.


#include "BowlingGameMode.h"

ABowlingGameMode::ABowlingGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABowlingGameMode::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APin::StaticClass(), FoundActors);
	PinCount = FoundActors.Num();

	if (APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0))
	{
		Ball = Cast<ABall>(PlayerPawn);
		if (Ball)
		{
			for (AActor* FoundActor : FoundActors)
			{
				if (APin* Pin = Cast<APin>(FoundActor))
				{
					Pins.Add(Pin);
				}
			}
		}

	}

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PlayerController)
	{
		ScreenMessageWidget = CreateWidget<UScreenMessage>(PlayerController, ScreenMessageClass);
		if (ScreenMessageWidget)
		{
			ScreenMessageWidget->AddToPlayerScreen();
			DrawScores();
		}
		else
		{
			UE_LOG(LogTemp, Display, TEXT("ABowlingGameMode::BeginPlay(): ScreenMessageWidget is null pointer!"));

		}
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("ABowlingGameMode::BeginPlay(): PlayerController is null pointer!"));
	}
}

void ABowlingGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Ball->IsBallRolling)
	{
		if (IsThrowFinished())
		{
			int32 RollScore = RollFinished();
			Roll(RollScore);
			DrawScores();
		}
	}

}

void ABowlingGameMode::ComputeScoresPass1()
{
	for (int32 i = 0; i < NumFrames + 2; ++i)
	{
		UFrameScore* NewFrame = NewObject<UFrameScore>(this);
		// NewFrame->SetRollScores(&RollScores);
		FrameScores.Add(NewObject<UFrameScore>(this));
	}

	int32 FrameNum = 0;
	int32 RollNumInFrame = 0;
	int32 RollIndex = 0;
	for (int32 RollScore : RollScores)
	{
		if (RollNumInFrame == 0)
		{
			FrameScores[FrameNum]->RollIndex = RollIndex;
			FrameScores[FrameNum]->Roll1Score = RollScore;
			if (RollScore == 10)
			{
				FrameScores[FrameNum]->Roll1String = TEXT("╳");
				FrameScores[FrameNum]->FrameFinished = true;
				FrameNum++;
				RollNumInFrame = 0;
			}
			else
			{
				if (RollScore == 0)
				{
					FrameScores[FrameNum]->Roll1String = FString::Printf(TEXT("-"));
				}
				else
				{
					FrameScores[FrameNum]->Roll1String = FString::Printf(TEXT("%d"), RollScore);
				}
				RollNumInFrame = 1;
			}
		}
		else
		{
			if (FrameScores[FrameNum]->Roll1Score + RollScore == 10)
			{
				FrameScores[FrameNum]->Roll2String = TEXT("╱");
			}
			else
			{
				if (RollScore == 0)
				{
					FrameScores[FrameNum]->Roll2String = FString::Printf(TEXT("-"));
				}
				else {
					FrameScores[FrameNum]->Roll2String = FString::Printf(TEXT("%d"), RollScore);
				}
			}

			FrameScores[FrameNum]->Roll2Score = RollScore;
			FrameScores[FrameNum]->FrameFinished = true;

			FrameNum++;
			RollNumInFrame = 0;
		}
		RollIndex++;
	}

}

int32 ABowlingGameMode::ComputeScoresPass2()
{
	// Now compute the frame scores for the frames for which that is possible
	// Only update the FrameScore objects for the real frames.
	int32 TotalScore = 0;
	for (int32 FrameNum = 0; FrameNum < NumFrames; FrameNum++)
	{
		UFrameScore* FrameScore = FrameScores[FrameNum];

		if (!FrameScore)
		{
			UE_LOG(LogTemp, Error, TEXT("FrameScore is null pointer!"));
			break;
		}

		if (!FrameScore->IsFinished())
		{
			break;
		}

		int32 PreviousFrameScore = 0;
		if (FrameNum == 0)
		{
			PreviousFrameScore = 0;
		}
		else
		{
			PreviousFrameScore = FrameScores[FrameNum - 1]->GetFrameScore();
		}


		if (FrameScore->IsOpen())
		{
			FrameScore->SetFrameScore(PreviousFrameScore + FrameScore->Roll1Score + FrameScore->Roll2Score);
		}
		else
		{
			if (FrameScore->IsSpare())
			{
				if (FrameScore->RollIndex + 2 < RollScores.Num())
				{
					FrameScore->SetFrameScore(PreviousFrameScore + 10 + RollScores[FrameScore->RollIndex + 2]);
				}
			}
			else // We have a Strike
			{
				if (FrameScore->RollIndex + 2 < RollScores.Num())
				{
					FrameScore->SetFrameScore(PreviousFrameScore + 10 + RollScores[FrameScore->RollIndex + 1] + RollScores[FrameScore->RollIndex + 2]);
				}

			}
		}
	}

	/*for (int32 FrameNum = 0; FrameNum < NumFrames; FrameNum++)
	{
		UE_LOG(LogTemp, Display, TEXT("\n--- Frame: %d ---"), FrameNum);
		UFrameScore* FrameScore = FrameScores[FrameNum];
		FrameScore->PrintFrameScore();

	}*/
	return TotalScore;
}

int32 ABowlingGameMode::Score()
{
	int32 TotalScore = 0;
	ComputeScoresPass1();
	TotalScore = ComputeScoresPass2();
	return TotalScore;
}

FString ABowlingGameMode::CreateSeparatorSting(FString LeftSep, FString InnerSep, FString RightSep) const
{
	FString SeparatorString;
	if (NumFrames == 1)
	{
		// last frame.
		FString part = FString::Printf(TEXT("%s───────%s"), *LeftSep, *RightSep);
		SeparatorString += part;
	}
	else
		for (int32 FrameNum = 0; FrameNum < NumFrames; FrameNum++)
		{
			if (FrameNum == 0)
			{
				FString part = FString::Printf(TEXT("%s─────"), *LeftSep);
				SeparatorString += part;
			}
			else
				if (FrameNum < NumFrames - 1)
				{
					FString part = FString::Printf(TEXT("%s─────"), *InnerSep);
					SeparatorString += part;
				}
				else
				{
					// last frame.
					FString part = FString::Printf(TEXT("%s───────%s"), *InnerSep, *RightSep);
					SeparatorString += part;
				}
		}

	return SeparatorString;
}

FString ABowlingGameMode::CreateFrameNumbersString() const
{
	FString FramesString;
	{
		for (int32 FrameNum = 0; FrameNum < NumFrames; FrameNum++)
		{
			if (FrameNum < NumFrames - 1)
			{
				FString part = FString::Printf(TEXT("│ %2d  "), FrameNum);
				FramesString += part;
			}
			else
			{
				// last frame.
				FString part = FString::Printf(TEXT("│  %2d   "), FrameNum);
				FramesString += part;
			}
		}
		FString part = FString::Printf(TEXT("│"));
		FramesString += part;
	}
	return FramesString;
}

FString ABowlingGameMode::CreateRollScoresString() const
{
	FString RollScoresString;
	{
		for (int32 FrameNum = 0; FrameNum < NumFrames; FrameNum++)
		{

			UFrameScore* FrameScore = FrameScores[FrameNum];

			FString part;
			if (FrameNum < NumFrames - 1)
			{
				part = FString::Printf(TEXT("│ %s %s "), *FrameScore->Roll1String, *FrameScore->Roll2String);
			}
			else
			{
				// last frame.
				if (FrameScore->Roll1Score + FrameScore->Roll2Score < 10)
				{
					// Not a Strike or Spare
					part = FString::Printf(TEXT("│ %s %s   │"), *FrameScore->Roll1String, *FrameScore->Roll2String);
				}
				else
				{
					// We have a Strike or a Spare
					UFrameScore* FrameScoreOneFurther = FrameScores[FrameNum + 1];

					if (FrameScore->Roll1Score < 10)
					{
						// The Spare case
						part = FString::Printf(TEXT("│ %s %s %s │"), *FrameScore->Roll1String, *FrameScore->Roll2String, *FrameScoreOneFurther->Roll1String);
					}
					else
					{
						// The Strike case
						if (FrameScoreOneFurther->Roll1Score < 10)
						{
							// Last frame was a Strike, followed by a non-strike
							part = FString::Printf(TEXT("│ %s %s %s │"), *FrameScore->Roll1String, *FrameScoreOneFurther->Roll1String, *FrameScoreOneFurther->Roll2String);
						}
						else
						{
							// Last frame was a Strike, followed by a Strike
							UFrameScore* FrameScoreTwoFurther = FrameScores[FrameNum + 2];
							part = FString::Printf(TEXT("│ %s %s %s │"), *FrameScore->Roll1String, *FrameScoreOneFurther->Roll1String, *FrameScoreTwoFurther->Roll2String);
						}
					}
				}
			}
			RollScoresString += part;
		}
	}
	return RollScoresString;
}

FString ABowlingGameMode::CreateFrameScoresString() const
{
	FString FrameScoresString;
	{
		for (int32 FrameNum = 0; FrameNum < NumFrames; FrameNum++)
		{
			FString part;

			UFrameScore* FrameScore = FrameScores[FrameNum];
			if (FrameScore->FrameHasScore)
			{
				if (FrameNum < NumFrames - 1)
				{
					part = FString::Printf(TEXT("│ %3d "), FrameScore->GetFrameScore());
				}
				else
				{
					part = FString::Printf(TEXT("│  %3d  │"), FrameScore->GetFrameScore());
				}
			}
			else
			{
				if (FrameNum < NumFrames - 1)
				{
					part = FString::Printf(TEXT("│     "));
				}
				else
				{
					part = FString::Printf(TEXT("│       │"));
				}
			}
			FrameScoresString += part;

		}
	}
	return FrameScoresString;
}


void ABowlingGameMode::DrawScores()
{
	// Update scores from the rolls
	Score();


	FString SeparatorTop = CreateSeparatorSting(TEXT("┌"), TEXT("┬"), TEXT("┐"));
	FString SeparatorInner = CreateSeparatorSting(TEXT("├"), TEXT("┼"), TEXT("┤"));
	FString SeparatorBottom = CreateSeparatorSting(TEXT("└"), TEXT("┴"), TEXT("┘"));
	FString FrameNumbersString = CreateFrameNumbersString();
	FString RollScoresString = CreateRollScoresString();
	FString FrameScoresString = CreateFrameScoresString();

	FString Message = FString::Printf(TEXT("%s\n%s\n%s\n%s\n%s\n%s\n%s"),
		*SeparatorTop,
		*FrameNumbersString,
		*SeparatorInner,
		*RollScoresString,
		*SeparatorInner,
		*FrameScoresString,
		*SeparatorBottom);

	ScreenMessageWidget->SetMessageText(Message);
}

bool ABowlingGameMode::IsThrowFinished()
{
	const float SleepThreshold2 = 25.0f; // square of 5.0 cm/s

	// Check the balls velocity (angular velocity is not stable).
	if (Ball->SphereComp->GetPhysicsLinearVelocity().SizeSquared() > SleepThreshold2)
	{
		return false;
	}

	// If any pin does still move the trow is not finished
	for (APin* Pin : Pins)
	{
		if (Pin->BaseMesh->IsSimulatingPhysics())
		{
			if (Pin->BaseMesh->GetPhysicsLinearVelocity().SizeSquared() > SleepThreshold2)
			{
				return false;
			}
		}
	}

	return true;
}


int32 ABowlingGameMode::RollFinished()
{

	Ball->IsBallRolling = false;

	// Check what pins are fallen or moved:
	int32 RollScore = 0;
	for (APin* Pin : Pins)
	{
		if (!Pin->IsRemoved && Pin->ShouldBeRemoved())
		{
			RollScore += Pin->ShouldBeRemoved();
			// Pin->ResetPinLocation();
			// Pin->AddActorLocalOffset(FVector(60.0f, 0.0f, 100.0f));
			Pin->IsRemoved = true;
		}
	}
	return RollScore;
}

void ABowlingGameMode::Roll(int32 RollScore)
{
	RollScores.Add(RollScore);

	// Update the number of pins scored in the current frame
	PinsInFrame += RollScore;

	if (FrameNumber < NumFrames - 1)
	{
		// all but the last frame
		if (PinsInFrame == 10 || RollNumberInFrame == 1)
		{
			PinsInFrame = 0;
			RollNumberInFrame = 0;
			FrameNumber++;
			TriggerFrameOver();
		}
		else {
			RollNumberInFrame = 1;
			TriggerRollOver();
		}
	}
	else
	{
		// last frame logic
		if (RollNumberInFrame == 1 && PinsInFrame < 10)
		{
			// There is no Strike or Spare, so we need to exit
			TriggerGameOver();
		}
		else
		{
			if (RollNumberInFrame == 2)
			{
				// We already got Rolls 0, 1, and are processing 2. There is no next roll
				TriggerGameOver();
			}
			else
			{
				// There is a Strike or a Spare in the last roll, so more rolls required.
				RollNumberInFrame++;
				if (PinsInFrame == 10 || PinsInFrame == 20) {
					TriggerFrameOver(); // Note, this is only a clean setup of the pins
				}
				else
				{
					TriggerRollOver();
				}
			}
		}
	}
}





// ################################################################
void ABowlingGameMode::TriggerRollOver()
{
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &ABowlingGameMode::OnRollOverTimerTimeout, GameOverDelay, false);
}

void ABowlingGameMode::OnRollOverTimerTimeout()
{
	if (Ball)
	{
		Ball->ResetBallLocation();
		for (APin* Pin : Pins)
		{
			if (Pin->IsRemoved)
			{
				Pin->ResetPinLocation();
				Pin->AddActorLocalOffset(FVector(60.0f, 0.0f, 100.0f));
			}
		}

	}
}

// ################################################################
void ABowlingGameMode::TriggerFrameOver()
{
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &ABowlingGameMode::OnFrameOverTimerTimeout, GameOverDelay, false);
}

void ABowlingGameMode::OnFrameOverTimerTimeout()
{
	if (Ball)
	{
		Ball->ResetBallLocation();
		for (APin* Pin : Pins)
		{
			Pin->ResetPinLocation();
			Pin->IsRemoved = false;
		}
	}
}

// ################################################################

void ABowlingGameMode::TriggerGameOver()
{
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &ABowlingGameMode::OnGameOverTimerTimeout, GameOverDelay, false);
}

void ABowlingGameMode::OnGameOverTimerTimeout()
{
	FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(GetWorld());
	UGameplayStatics::OpenLevel(GetWorld(), *CurrentLevelName);

	if (Ball)
	{
		Ball->ResetBallLocation();
	}
}


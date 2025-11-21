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
	UE_LOG(LogTemp, Display, TEXT("#Pins: %d"), PinCount);

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
    if(PlayerController)
    {
        ScreenMessageWidget = CreateWidget<UScreenMessage>(PlayerController, ScreenMessageClass);
        if (ScreenMessageWidget)
        {
            ScreenMessageWidget->AddToPlayerScreen();
            // ScreenMessageWidget->SetMessageText("+-----+-----+-----+-----+-----+-----+-----+-----+-----+-------+\n|  1  |  2  |  3  |  4  |  5  |  6  |  7  |  8  |  9  |  10   |\n+-----+-----+-----+-----+-----+-----+-----+-----+-----+-------+\n| 3 6 | 1 1 | 1 1 | 1 / | 0 / | X   | 1 1 | 1 1 | 1 1 | 1 / 6 |\n+-----+-----+-----+-----+-----+-----+-----+-----+-----+-------+\n|  9  | 11  | 13  | 23  | 43  | 55  | 57  | 59  | 61  |  77   |\n+-----+-----+-----+-----+-----+-----+-----+-----+-----+-------+\n");
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

void ABowlingGameMode::DrawScores()
{
    
    // Create an Array of FrameScores (A 4D vector per entry) and initielize to zero.
    // Note, the vector is two longer than the actual number of game frames.
    for (int32 i = 0; i < NumFrames + 2; ++i)
    {
        UFrameScore* NewFrame = NewObject<UFrameScore>(this);
        FrameScores.Add(NewObject<UFrameScore>(this));
    }

    {
        int32 FrameNum = 0;
        int32 RollNumInFrame = 0;
        for (int32 RollScore : RollScores)
        {
            if (RollNumInFrame == 0)
            {
                FrameScores[FrameNum]->Roll1Score = RollScore;
                if (RollScore == 10)
                {
                    FrameScores[FrameNum]->Roll1String = TEXT("╳");
                    FrameNum++;
                    RollNumInFrame = 0;
                }
                else
                {
                    FrameScores[FrameNum]->Roll1String = FString::Printf(TEXT("%d"), RollScore);
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
                    FrameScores[FrameNum]->Roll2String = FString::Printf(TEXT("%d"), RollScore);
                }

                FrameScores[FrameNum]->Roll2Score = RollScore;
                FrameNum++;
                RollNumInFrame = 0;
            }
        }
    }

    
    FString RollScoresString;
    {
        for (int32 FrameNum = 0; FrameNum < NumFrames; FrameNum++)
        {

            UFrameScore* FrameScore = FrameScores[FrameNum];
            FString part = FString::Printf(TEXT("│ %s %s "), *FrameScore->Roll1String, *FrameScore->Roll2String);
            RollScoresString += part;
            if (FrameNum == NumFrames - 1)
            {
                // last frame.
                part = FString::Printf(TEXT("? "));
                RollScoresString += part;
            }
        }
        FString part = FString::Printf(TEXT("│"));
        RollScoresString += part;
    }

    
    FString Characters1 = FString::Printf(TEXT("┌─┬─┐"));
    FString Characters2 = FString::Printf(TEXT("│ │ │"));
    FString Characters3 = FString::Printf(TEXT("├─┼─┤"));
    FString Characters4 = FString::Printf(TEXT("│ │ │"));
    FString Characters5 = FString::Printf(TEXT("└─┴─┘"));
    FString Characters6 = FString::Printf(TEXT("╱ ╳ -"));


    FString SeparatorTop     = FString::Printf(TEXT("┌─────┬─────┬─────┬─────┬─────┬─────┬─────┬─────┬─────┬───────┐"));
    FString SeparatorInner   = FString::Printf(TEXT("├─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼───────┤"));
    FString SeparatorBottom  = FString::Printf(TEXT("└─────┴─────┴─────┴─────┴─────┴─────┴─────┴─────┴─────┴───────┘"));

    FString FramesString     = FString::Printf(TEXT("│  1  │  2  │  3  │  4  │  5  │  6  │  7  │  8  │  9  │  10   │"));
    // FString RollScoresString = FString::Printf(TEXT("│     │     │     │     │     │     │     │     │     │       │"));
    FString FrameScoreString = FString::Printf(TEXT("│     │     │     │     │     │     │     │     │     │       │"));
    FString Message = FString::Printf(TEXT("%s\n%s\n%s\n%s\n%s\n%s\n%s"), *SeparatorTop, *FramesString, *SeparatorInner, *RollScoresString, *SeparatorInner, *FrameScoreString, *SeparatorBottom);

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
    UE_LOG(LogTemp, Display, TEXT("In Roll(): FrameNumber: %d  RollNumberInFrame: %d  PinsInFrame: %d"), FrameNumber, RollNumberInFrame, PinsInFrame);

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
        if (RollNumberInFrame == 1 && PinsInFrame < 10)
        {
            // There is no Strike or Spare, so we need to exit
            TriggerGameOver();
        }
        else
        {
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

    UE_LOG(LogTemp, Display, TEXT("RollScore: %d"), RollScore);



}

int32 ABowlingGameMode::Score() const
{
    return TotalScore;
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


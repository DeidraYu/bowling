// Fill out your copyright notice in the Description page of Project Settings.


#include "FrameScore.h"

//void UFrameScore::SetRollScores(TArray<int32>* InRollScores)
//{
//	RollScores = InRollScores;
//}


bool UFrameScore::IsOpen()
{
	if (!FrameFinished)
	{
		return false;
	}

	if (Roll1Score < 10 && Roll1Score + Roll2Score < 10)
	{
		return true;
	}
	else
	{
		return false;
	}
}

int32 UFrameScore::GetFrameScore() const
{
	return Score;
}

void UFrameScore::SetFrameScore(int32 NewFrameScore)
{
	Score = NewFrameScore;
	FrameHasScore = true;
}

bool UFrameScore::IsSpare()
{
	if (!FrameFinished)
	{
		return false;
	}

	if (Roll1Score < 10 && Roll1Score + Roll2Score == 10)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool UFrameScore::IsStrike()
{
	if (!FrameFinished)
	{
		return false;
	}

	return Roll1Score == 10;
}

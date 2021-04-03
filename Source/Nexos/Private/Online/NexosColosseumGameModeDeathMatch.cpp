// Fill out your copyright notice in the Description page of Project Settings.


#include "Online/NexosColosseumGameModeDeathMatch.h"
#include "Nexos.h"
#include "NexosGameInstance.h"
#include "UI/NexosHUD.h"
#include "Player/NexosSpectatorPawn.h"
#include "Player/NexosPlayerCharacter.h"
#include "Online/NexosColosseumGameState.h"
#include "Online/NexosPlayerState.h"
#include "Online/NexosGameSession.h"

ANexosColosseumGameModeDeathMatch::ANexosColosseumGameModeDeathMatch(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bDelayedStart = false
		;
}

void ANexosColosseumGameModeDeathMatch::DetermineMatchWinner()
{
	ANexosColosseumGameState const* const MyGameState = CastChecked<ANexosColosseumGameState>(GameState);
	float BestScore = MIN_flt;
	int32 BestPlayer = -1;
	int32 NumBestPlayers = 0;

	for (int32 i = 0; i < MyGameState->PlayerArray.Num(); i++)
	{
		const float PlayerScore = MyGameState->PlayerArray[i]->GetScore();
		if (BestScore < PlayerScore)
		{
			BestScore = PlayerScore;
			BestPlayer = i;
			NumBestPlayers = 1;
		}
		else if (BestScore == PlayerScore)
		{
			NumBestPlayers++;
		}
	}

	WinnerPlayerState = (NumBestPlayers == 1) ? Cast<ANexosPlayerState>(MyGameState->PlayerArray[BestPlayer]) : NULL;
}

bool ANexosColosseumGameModeDeathMatch::IsWinner(ANexosPlayerState* PlayerState) const
{
	return PlayerState && !PlayerState->IsQuitter() && PlayerState == WinnerPlayerState;
}

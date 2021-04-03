// Fill out your copyright notice in the Description page of Project Settings.


#include "Online/NexosColosseumGameState.h"
#include "Nexos.h"
#include "Online/NexosColosseumGameModeBase.h"
#include "Online/NexosPlayerState.h"
#include "NexosGameInstance.h"

ANexosColosseumGameState::ANexosColosseumGameState(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	NumTeams = 0;
	RemainingTime = 0;
	bTimerPaused = false;
}

void ANexosColosseumGameState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ANexosColosseumGameState, NumTeams);
	DOREPLIFETIME(ANexosColosseumGameState, RemainingTime);
	DOREPLIFETIME(ANexosColosseumGameState, bTimerPaused);
	DOREPLIFETIME(ANexosColosseumGameState, TeamScores);
}

void ANexosColosseumGameState::GetRankedMap(int32 TeamIndex, RankedPlayerMap& OutRankedMap) const
{
	OutRankedMap.Empty();

	//first, we need to go over all the PlayerStates, grab their score, and rank them
	TMultiMap<int32, ANexosPlayerState*> SortedMap;
	for (int32 i = 0; i < PlayerArray.Num(); ++i)
	{
		int32 Score = 0;
		ANexosPlayerState* CurPlayerState = Cast<ANexosPlayerState>(PlayerArray[i]);
		if (CurPlayerState && (CurPlayerState->GetTeamNum() == TeamIndex))
		{
			SortedMap.Add(FMath::TruncToInt(CurPlayerState->GetScore()), CurPlayerState);
		}
	}

	//sort by the keys
	SortedMap.KeySort(TGreater<int32>());

	//now, add them back to the ranked map
	OutRankedMap.Empty();

	int32 Rank = 0;
	for (TMultiMap<int32, ANexosPlayerState*>::TIterator It(SortedMap); It; ++It)
	{
		OutRankedMap.Add(Rank++, It.Value());
	}

}


void ANexosColosseumGameState::RequestFinishAndExitToMainMenu()
{
	if (AuthorityGameMode)
	{
		// we are server, tell the gamemode
		ANexosColosseumGameState* const GameMode = Cast<ANexosColosseumGameState>(AuthorityGameMode);
		if (GameMode)
		{
			GameMode->RequestFinishAndExitToMainMenu();
		}
	}
	else
	{
		ANexosPlayerController* const PrimaryPC = Cast<ANexosPlayerController>(GetGameInstance()->GetFirstLocalPlayerController());
		if (PrimaryPC)
		{
			PrimaryPC->HandleReturnToMainMenu();
		}
	}

}


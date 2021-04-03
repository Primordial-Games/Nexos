// Fill out your copyright notice in the Description page of Project Settings.


#include "Online/NexosColosseumGameModeTDM.h"
#include "Nexos.h"
#include "NexosGameInstance.h"
#include "UI/NexosHUD.h"
#include "Player/NexosSpectatorPawn.h"
#include "Player/NexosPlayerCharacter.h"
#include "Online/NexosColosseumGameState.h"
#include "Online/NexosPlayerState.h"
#include "Online/NexosGameSession.h"
#include "Bots/ShooterAIController.h"

ANexosColosseumGameModeTDM::ANexosColosseumGameModeTDM(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	NumTeams = 2;
	bDelayedStart = false;
}

void ANexosColosseumGameModeTDM::PostLogin(APlayerController* NewPlayer)
{
	// Place player on a team before Super (VoIP team based init, findplayerstart, etc)
	ANexosPlayerState* NewPlayerState = CastChecked<ANexosPlayerState>(NewPlayer->PlayerState);
	const int32 TeamNum = ChooseTeam(NewPlayerState);
	NewPlayerState->SetTeamNum(TeamNum);

	Super::PostLogin(NewPlayer);
}

void ANexosColosseumGameModeTDM::InitGameState()
{
	Super::InitGameState();

	ANexosColosseumGameState* const MyGameState = Cast<ANexosColosseumGameState>(GameState);
	if (MyGameState)
	{
		MyGameState->NumTeams = NumTeams;
	}
}

bool ANexosColosseumGameModeTDM::CanDealDamage(ANexosPlayerState* DamageInstigator, class ANexosPlayerState* DamagedPlayer) const
{
	return DamageInstigator && DamagedPlayer && (DamagedPlayer == DamageInstigator || DamagedPlayer->GetTeamNum() != DamageInstigator->GetTeamNum());
}

int32 ANexosColosseumGameModeTDM::ChooseTeam(ANexosPlayerState* ForPlayerState) const
{
	TArray<int32> TeamBalance;
	TeamBalance.AddZeroed(NumTeams);

	// get current team balance
	for (int32 i = 0; i < GameState->PlayerArray.Num(); i++)
	{
		ANexosPlayerState const* const TestPlayerState = Cast<ANexosPlayerState>(GameState->PlayerArray[i]);
		if (TestPlayerState && TestPlayerState != ForPlayerState && TeamBalance.IsValidIndex(TestPlayerState->GetTeamNum()))
		{
			TeamBalance[TestPlayerState->GetTeamNum()]++;
		}
	}

	// find least populated one
	int32 BestTeamScore = TeamBalance[0];
	for (int32 i = 1; i < TeamBalance.Num(); i++)
	{
		if (BestTeamScore > TeamBalance[i])
		{
			BestTeamScore = TeamBalance[i];
		}
	}

	// there could be more than one...
	TArray<int32> BestTeams;
	for (int32 i = 0; i < TeamBalance.Num(); i++)
	{
		if (TeamBalance[i] == BestTeamScore)
		{
			BestTeams.Add(i);
		}
	}

	// get random from best list
	const int32 RandomBestTeam = BestTeams[FMath::RandHelper(BestTeams.Num())];
	return RandomBestTeam;
}

void ANexosColosseumGameModeTDM::DetermineMatchWinner()
{
	ANexosColosseumGameState const* const MyGameState = Cast<ANexosColosseumGameState>(GameState);
	int32 BestScore = MIN_uint32;
	int32 BestTeam = -1;
	int32 NumBestTeams = 1;

	for (int32 i = 0; i < MyGameState->TeamScores.Num(); i++)
	{
		const int32 TeamScore = MyGameState->TeamScores[i];
		if (BestScore < TeamScore)
		{
			BestScore = TeamScore;
			BestTeam = i;
			NumBestTeams = 1;
		}
		else if (BestScore == TeamScore)
		{
			NumBestTeams++;
		}
	}

	WinnerTeam = (NumBestTeams == 1) ? BestTeam : NumTeams;
}

bool ANexosColosseumGameModeTDM::IsWinner(ANexosPlayerState* PlayerState) const
{
	return PlayerState && !PlayerState->IsQuitter() && PlayerState->GetTeamNum() == WinnerTeam;
}

bool ANexosColosseumGameModeTDM::IsSpawnpointAllowed(APlayerStart* SpawnPoint, AController* Player) const
{
	if (Player)
	{
		ANexosTeamStart* TeamStart = Cast<ANexosTeamStart>(SpawnPoint);
		ANexosPlayerState* PlayerState = Cast<ANexosPlayerState>(Player->PlayerState);

		if (PlayerState && TeamStart && TeamStart->SpawnTeam != PlayerState->GetTeamNum())
		{
			return false;
		}
	}

	return Super::IsSpawnpointAllowed(SpawnPoint, Player);
}

void ANexosColosseumGameModeTDM::InitBot(AShooterAIController* AIC, int32 BotNum)
{
	ANexosPlayerState* BotPlayerState = CastChecked<ANexosPlayerState>(AIC->PlayerState);
	const int32 TeamNum = ChooseTeam(BotPlayerState);
	BotPlayerState->SetTeamNum(TeamNum);

	Super::InitBot(AIC, BotNum);
}

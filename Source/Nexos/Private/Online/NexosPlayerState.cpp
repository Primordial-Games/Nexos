// Copyright Epic Games, Inc. All Rights Reserved.

#include "Online/NexosPlayerState.h"
#include "Nexos.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Online/NexosColosseumGameState.h"
#include "Player/NexosPlayerController.h"
#include "Player/NexosPlayerCharacter.h"
#include "Net/OnlineEngineInterface.h"

ANexosPlayerState::ANexosPlayerState(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	TeamNumber = 0;
	NumKills = 0;
	NumDeaths = 0;
	NumBulletsFired = 0;
	bQuitter = false;
	UniquePlayerId = OnlineInterface->GetIdentityInterface()->GetAuthToken(0);
}

void ANexosPlayerState::Reset()
{
	Super::Reset();

	//PlayerStates persist across seamless travel.  Keep the same teams as previous match.
	SetTeamNum(0);
	NumKills = 0;
	NumDeaths = 0;
	NumBulletsFired = 0;
	bQuitter = false;
}

void ANexosPlayerState::RegisterPlayerWithSession(bool bWasFromInvite)
{
	if (UOnlineEngineInterface::Get()->DoesSessionExist(GetWorld(), NAME_GameSession))
	{
		Super::RegisterPlayerWithSession(bWasFromInvite);
	}
}

void ANexosPlayerState::UnregisterPlayerWithSession()
{
	if (!IsFromPreviousLevel() && UOnlineEngineInterface::Get()->DoesSessionExist(GetWorld(), NAME_GameSession))
	{
		Super::UnregisterPlayerWithSession();
	}
}

void ANexosPlayerState::ClientInitialize(AController* InController)
{
	Super::ClientInitialize(InController);

	UpdateTeamColors();
}

void ANexosPlayerState::SetTeamNum(int32 NewTeamNumber)
{
	TeamNumber = NewTeamNumber;

	UpdateTeamColors();
}

void ANexosPlayerState::OnRep_TeamColor()
{
	UpdateTeamColors();
}

void ANexosPlayerState::AddBulletsFired(int32 NumBullets)
{
	NumBulletsFired += NumBullets;
}



void ANexosPlayerState::SetQuitter(bool bInQuitter)
{
	bQuitter = bInQuitter;
}

void ANexosPlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);

	ANexosPlayerState* NexosPlayer = Cast<ANexosPlayerState>(PlayerState);
	if (NexosPlayer)
	{
		NexosPlayer->TeamNumber = TeamNumber;
	}
}

void ANexosPlayerState::UpdateTeamColors()
{
	AController* OwnerController = Cast<AController>(GetOwner());
	if (OwnerController != NULL)
	{
		ANexosPlayerCharacter* NexosCharacter = Cast<ANexosPlayerCharacter>(OwnerController->GetCharacter());
		if (NexosCharacter != NULL)
		{
			NexosCharacter->UpdateTeamColorsAllMIDs();
		}
	}
}

int32 ANexosPlayerState::GetTeamNum() const
{
	return TeamNumber;
}

int32 ANexosPlayerState::GetKills() const
{
	return NumKills;
}

int32 ANexosPlayerState::GetDeaths() const
{
	return NumDeaths;
}

int32 ANexosPlayerState::GetNumBulletsFired() const
{
	return NumBulletsFired;
}

bool ANexosPlayerState::IsQuitter() const
{
	return bQuitter;
}

void ANexosPlayerState::ScoreKill(ANexosPlayerState* Victim, int32 Points)
{
	NumKills++;
	ScorePoints(Points);
}

void ANexosPlayerState::ScoreDeath(ANexosPlayerState* KilledBy, int32 Points)
{
	NumDeaths++;
	ScorePoints(Points);
}

void ANexosPlayerState::ScorePoints(int32 Points)
{
	ANexosColosseumGameState* const MyGameState = GetWorld()->GetGameState<ANexosColosseumGameState>();
	if (MyGameState && TeamNumber >= 0)
	{
		if (TeamNumber >= MyGameState->TeamScores.Num())
		{
			MyGameState->TeamScores.AddZeroed(TeamNumber - MyGameState->TeamScores.Num() + 1);
		}

		MyGameState->TeamScores[TeamNumber] += Points;
	}

	SetScore(GetScore() + Points);
}

void ANexosPlayerState::InformAboutKill_Implementation(class ANexosPlayerState* KillerPlayerState, const UDamageType* KillerDamageType, class ANexosPlayerState* KilledPlayerState)
{
	//id can be null for bots
	if (KillerPlayerState->GetUniqueId().IsValid())
	{
		//search for the actual killer before calling OnKill()	
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			ANexosPlayerController* TestPC = Cast<ANexosPlayerController>(*It);
			if (TestPC && TestPC->IsLocalController())
			{
				// a local player might not have an ID if it was created with CreateDebugPlayer.
				ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(TestPC->Player);
				FUniqueNetIdRepl LocalID = LocalPlayer->GetCachedUniqueNetId();
				if (LocalID.IsValid() && *LocalPlayer->GetCachedUniqueNetId() == *KillerPlayerState->GetUniqueId())
				{
					TestPC->OnKill();
				}
			}
		}
	}
}

void ANexosPlayerState::BroadcastDeath_Implementation(class ANexosPlayerState* KillerPlayerState, const UDamageType* KillerDamageType, class ANexosPlayerState* KilledPlayerState)
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		// all local players get death messages so they can update their huds.
		ANexosPlayerController* TestPC = Cast<ANexosPlayerController>(*It);
		if (TestPC && TestPC->IsLocalController())
		{
			TestPC->OnDeathMessage(KillerPlayerState, this, KillerDamageType);
		}
	}
}

void ANexosPlayerState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ANexosPlayerState, TeamNumber);
	DOREPLIFETIME(ANexosPlayerState, NumKills);
	DOREPLIFETIME(ANexosPlayerState, NumDeaths);
}

FString ANexosPlayerState::GetShortPlayerName() const
{
	if (GetPlayerName().Len() > MAX_PLAYER_NAME_LENGTH)
	{
		return GetPlayerName().Left(MAX_PLAYER_NAME_LENGTH) + "...";
	}
	return GetPlayerName();
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "Online/NexosPrivateLobbyGameMode.h"
#include "Online/NexosGameSession.h"
#include "Online/NexosColosseumGameState.h"
#include "Player/NexosLobbyPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Common/UdpSocketBuilder.h"
#include "PlayFab.h"
#include "Core/PlayFabClientAPI.h"
#include "Core/PlayFabMultiplayerAPI.h"
#include "NexosGameInstance.h"



ANexosPrivateLobbyGameMode::ANexosPrivateLobbyGameMode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	
}


void ANexosPrivateLobbyGameMode::RestartPlayer(class AController* NewPlayer)
{
	// don't restart
}

/** Returns game session class to use */
TSubclassOf<AGameSession> ANexosPrivateLobbyGameMode::GetGameSessionClass() const
{
	return ANexosGameSession::StaticClass();
}

void ANexosPrivateLobbyGameMode::JoinPlayFabServer(FString ServerURL)
{
	ProcessServerTravel(ServerURL, false);

	UNexosGameInstance* NexosGI = Cast<UNexosGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

	PlayFab::MultiplayerModels::FMatchmakingPlayer CreatorEntity;
	

	PlayFab::MultiplayerModels::FCreateMatchmakingTicketRequest request;

}

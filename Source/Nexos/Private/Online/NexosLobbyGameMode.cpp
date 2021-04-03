// Fill out your copyright notice in the Description page of Project Settings.


#include "Online/NexosLobbyGameMode.h"
#include "Player/NexosLobbyPlayerController.h"
#include "Online/NexosGameSession.h"
#include "Core/PlayFabMultiplayerAPI.h"


ANexosLobbyGameMode::ANexosLobbyGameMode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void ANexosLobbyGameMode::BeginPlay()
{
	Super::BeginPlay();

	multiplayerAPI = IPlayFabModuleInterface::Get().GetMultiplayerAPI();
}

void ANexosLobbyGameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	Super::PreLogin(Options, Address, FUniqueNetIdRepl(), ErrorMessage);
}

void ANexosLobbyGameMode::RequestExit()
{
	FGenericPlatformMisc::RequestExit(false);
}

//Create a Backfill ticket
void ANexosLobbyGameMode::CreateBackfillTicket()
{
	PlayFab::MultiplayerModels::FCreateServerBackfillTicketRequest request;
	request.GiveUpAfterSeconds = 500;
	request.Members = PlayersInLobby;
	request.QueueName = QueueName;

	multiplayerAPI->CreateServerBackfillTicket(request, 
		PlayFab::UPlayFabMultiplayerAPI::FCreateServerBackfillTicketDelegate::CreateUObject(this, &ANexosLobbyGameMode::OnBackfillCreateSuccess),
		PlayFab::FPlayFabErrorDelegate::CreateUObject(this, &ANexosLobbyGameMode::OnBackfillCreatError));
}
void ANexosLobbyGameMode::OnBackfillCreateSuccess(const PlayFab::MultiplayerModels::FCreateServerBackfillTicketResult& Result) const
{
	BackfillTicketId = Result.TicketId;
}
void ANexosLobbyGameMode::OnBackfillCreatError(const PlayFab::FPlayFabCppError& ErrorResult) const
{
}

//Cancel a Backfill ticket
void ANexosLobbyGameMode::CancelBackfillTicket()
{
	PlayFab::MultiplayerModels::FCancelServerBackfillTicketRequest request;
	request.QueueName = QueueName;
	request.TicketId = BackfillTicketId;

	multiplayerAPI->CancelServerBackfillTicket(request,
		PlayFab::UPlayFabMultiplayerAPI::FCancelServerBackfillTicketDelegate::CreateUObject(this, &ANexosLobbyGameMode::OnBackfillCancelSuccess),
		PlayFab::FPlayFabErrorDelegate::CreateUObject(this, &ANexosLobbyGameMode::OnBackfillCancelError));
}
void ANexosLobbyGameMode::OnBackfillCancelSuccess(const PlayFab::MultiplayerModels::FCancelServerBackfillTicketResult& Result) const
{
}
void ANexosLobbyGameMode::OnBackfillCancelError(const PlayFab::FPlayFabCppError& ErrorResult) const
{
}

void ANexosLobbyGameMode::BeginMatch()
{
	GetWorld()->ServerTravel(TravelCommand);
}

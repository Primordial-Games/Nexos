// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/NexosLobbyPlayerController.h"
#include "Online/NexosPrivateLobbyGameMode.h"
#include "Online/NexosColosseumGameState.h"
#include "NexosGameInstance.h"
#include "WidgetController.h"
#include "Nexos.h"
#include "Blueprint/UserWidget.h"
#include "UI/InviteWidget.h"


ANexosLobbyPlayerController::ANexosLobbyPlayerController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void ANexosLobbyPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ANexosLobbyPlayerController::PlayerRegionLatency()
{

}

void ANexosLobbyPlayerController::ServerTravelToMap(FString MapName)
{
	ClientTravel(MapName, ETravelType::TRAVEL_Absolute, false);
}


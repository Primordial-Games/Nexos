// Fill out your copyright notice in the Description page of Project Settings.

#include "Online/NexosOnlineSessionClient.h"
#include "Nexos.h"
#include "NexosGameInstance.h"

UNexosOnlineSessionClient::UNexosOnlineSessionClient()
{
}

void UNexosOnlineSessionClient::OnSessionUserInviteAccepted(
	const bool							bWasSuccess,
	const int32							ControllerId,
	TSharedPtr< const FUniqueNetId > 	UserId,
	const FOnlineSessionSearchResult& InviteResult
)
{
}

void UNexosOnlineSessionClient::OnPlayTogetherEventReceived(int32 UserIndex, TArray<TSharedPtr<const FUniqueNetId>> UserIdList)
{

}
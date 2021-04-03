// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/NexosLocalPlayer.h"

UNexosLocalPlayer::UNexosLocalPlayer(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UNexosLocalPlayer::SetControllerId(int32 NewControllerId)
{
}

FString UNexosLocalPlayer::GetNickname() const
{
	return FString();
}

UNexosPersistentUser* UNexosLocalPlayer::GetPersistentUser() const
{
	return nullptr;
}

void UNexosLocalPlayer::LoadPersistentUser()
{
}

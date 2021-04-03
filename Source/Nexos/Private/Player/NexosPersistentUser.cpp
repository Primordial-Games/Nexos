// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/NexosPersistentUser.h"

UNexosPersistentUser::UNexosPersistentUser(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetToDefaults();
}

void UNexosPersistentUser::SaveIfDirty()
{
}

void UNexosPersistentUser::AddMatchResults(int32 MatchKills, int32 MatchDeaths, int32 MatchBulletsFired, bool bIsMatchWinner)
{
}

void UNexosPersistentUser::TellInputAboutKeybindings()
{
}

int32 UNexosPersistentUser::GetUserIndex() const
{
	return int32();
}

void UNexosPersistentUser::SetVibration(bool bVibration)
{
}

void UNexosPersistentUser::SetInvertedYAxis(bool bInvert)
{
}

void UNexosPersistentUser::SetAimSensitivity(float InSensitivity)
{
}

void UNexosPersistentUser::SetGamma(float InGamma)
{
}

void UNexosPersistentUser::SetIsRecordingDemos(const bool InbIsRecordingDemos)
{
}

void UNexosPersistentUser::SetToDefaults()
{
}

bool UNexosPersistentUser::IsAimSensitivityDirty() const
{
	return false;
}

bool UNexosPersistentUser::IsInvertedYAxisDirty() const
{
	return false;
}

void UNexosPersistentUser::SavePersistentUser()
{
}

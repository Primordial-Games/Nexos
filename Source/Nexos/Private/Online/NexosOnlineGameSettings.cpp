// Fill out your copyright notice in the Description page of Project Settings.

#include "Online/NexosOnlineGameSettings.h"
#include "Nexos.h"


FNexosOnlineSessionSettings::FNexosOnlineSessionSettings(bool bIsLAN, bool bIsPresence, int32 MaxNumPlayers)
{

}

FNexosOnlineSearchSettings::FNexosOnlineSearchSettings(bool bSearchingLAN, bool bSearchingPresence)
{

}

FNexosOnlineSearchSettingsEmptyDedicated::FNexosOnlineSearchSettingsEmptyDedicated(bool bSearchingLAN, bool bSearchingPresence) :
	FNexosOnlineSearchSettings(bSearchingLAN, bSearchingPresence)
{

}
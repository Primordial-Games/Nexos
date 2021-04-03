// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "OnlineSessionSettings.h"

class FNexosOnlineSessionSettings : public FOnlineSessionSettings
{
public:

	FNexosOnlineSessionSettings(bool bIsLAN = false, bool bIsPresence = false, int32 MaxNumPlayers = 4);
	virtual ~FNexosOnlineSessionSettings() {}
};

/**
 * General search setting for a Shooter game
 */
class FNexosOnlineSearchSettings : public FOnlineSessionSearch
{
public:
	FNexosOnlineSearchSettings(bool bSearchingLAN = false, bool bSearchingPresence = false);

	virtual ~FNexosOnlineSearchSettings() {}
};

/**
 * Search settings for an empty dedicated server to host a match
 */
class FNexosOnlineSearchSettingsEmptyDedicated : public FNexosOnlineSearchSettings
{
public:
	FNexosOnlineSearchSettingsEmptyDedicated(bool bSearchingLAN = false, bool bSearchingPresence = false);

	virtual ~FNexosOnlineSearchSettingsEmptyDedicated() {}
};
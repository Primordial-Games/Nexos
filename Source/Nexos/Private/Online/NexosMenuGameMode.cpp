// Fill out your copyright notice in the Description page of Project Settings.


#include "Online/NexosMenuGameMode.h"
#include "Player/NexosPlayerController_Menu.h"
#include "Online/NexosGameSession.h"

ANexosMenuGameMode::ANexosMenuGameMode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PlayerControllerClass = ANexosPlayerController_Menu::StaticClass();
	bUseSeamlessTravel = true;
}

void ANexosMenuGameMode::RestartPlayer(class AController* NewPlayer)
{
	// don't restart
}

/** Returns game session class to use */
TSubclassOf<AGameSession> ANexosMenuGameMode::GetGameSessionClass() const
{
	return ANexosGameSession::StaticClass();
}

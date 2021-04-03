// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "NexosMenuGameMode.generated.h"

/**
 * 
 */
UCLASS()
class NEXOS_API ANexosMenuGameMode : public AGameModeBase
{
	GENERATED_UCLASS_BODY()

public:

	// Begin AGameModeBase interface
	/** skip it, menu doesn't require player start or pawn */
	virtual void RestartPlayer(class AController* NewPlayer) override;

	/** Returns game session class to use */
	virtual TSubclassOf<AGameSession> GetGameSessionClass() const override;
	// End AGameModeBase interface

protected:

	/** Perform some final tasks before hosting/joining a session. Remove menus, set king state etc */
	void BeginSession();

	/** Display a loading screen */
	void ShowLoadingScreen();
	
};

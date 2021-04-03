// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Player/NexosLobbyPlayerController.h"
#include "PlayFab.h"
#include "Core/PlayFabError.h"
#include "Core/PlayFabMultiplayerDataModels.h"
#include "NexosPrivateLobbyGameMode.generated.h"


/**
 * 
 */
UCLASS(Blueprintable)
class NEXOS_API ANexosPrivateLobbyGameMode : public AGameModeBase
{
	GENERATED_UCLASS_BODY()
	
public:

	// Begin AGameModeBase interface
	/** skip it, menu doesn't require player start or pawn */
	virtual void RestartPlayer(class AController* NewPlayer) override;

	/** Returns game session class to use */
	virtual TSubclassOf<AGameSession> GetGameSessionClass() const override;
	// End AGameModeBase interface

	void CreateLobby();

	void FinishLobbyCreation();

	void DestroyLobby();

	void OnPlayerJoinedLobby();

	void OnPlayerLeftLobby();

	void OnHostMigration();

	void BeginMatchmakingSearch();

	UFUNCTION(BlueprintCallable, Category = "Online Services")
		void JoinPlayFabServer(FString ServerURL);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LobbyInfo)
		TArray<FString> Usernames;

	PlayFabMultiplayerPtr multiplayerAPI = nullptr;


protected:

	/** Perform some final tasks before hosting/joining a session. Remove menus, set king state etc */
	void BeginSession();

	/** Display a loading screen */
	void ShowLoadingScreen();

};

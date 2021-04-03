// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PlayFab.h"
#include "Core/PlayFabError.h"
#include "Core/PlayFabMultiplayerDataModels.h"
#include "NexosLobbyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class NEXOS_API ANexosLobbyGameMode : public AGameMode
{
	GENERATED_UCLASS_BODY()

public:
	virtual void BeginPlay() override;
	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
	
	//Log a player in and add them to the player list
	//virtual void PostLogin(APlayerController* NewPlayer) override;
	
	/** Initialize the game. This is called befor the actors' start */
	//virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	UFUNCTION(BlueprintCallable, Category = "Online Services")
		void RequestExit();

	
	UFUNCTION(BlueprintCallable, Category = "Online Services")
		void CreateBackfillTicket();
	void OnBackfillCreateSuccess(const PlayFab::MultiplayerModels::FCreateServerBackfillTicketResult& Result) const;
	void OnBackfillCreatError(const PlayFab::FPlayFabCppError& ErrorResult) const;

	UFUNCTION(BlueprintCallable, Category = "Online Services")
		void CancelBackfillTicket();
	void OnBackfillCancelSuccess(const PlayFab::MultiplayerModels::FCancelServerBackfillTicketResult& Result) const;
	void OnBackfillCancelError(const PlayFab::FPlayFabCppError& ErrorResult) const;

	UFUNCTION(BlueprintCallable, Category = "Match Functions")
		void BeginMatch();

	//UFUNCTION(BlueprintCallable, Category = "Match Functions")
		//void BeginFinalCountdown();

	//UFUNCTION(BlueprintCallable, Category = "Match Functions")
		//void CancelFinalCountdown();

	//UFUNCTION(BlueprintCallable, Category = "Match Functions")
		//void CheckPlayerCount();


	UPROPERTY(config)
		FString QueueName;

	UPROPERTY()
		mutable FString BackfillTicketId;

	UPROPERTY()
		FString TravelCommand;

private:
	PlayFabMultiplayerPtr multiplayerAPI = nullptr;

	TArray<PlayFab::MultiplayerModels::FMatchmakingPlayerWithTeamAssignment> PlayersInLobby;
};

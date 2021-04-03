// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "Player/NexosPlayerController.h"
#include "GameFramework/PlayerStart.h"
#include "NexosColosseumGameModeBase.generated.h"

class AShooterAIController;
class ANexosPlayerState;
class ANexosPickup;
class FUniqueNetId;

UCLASS(config = Game)
class NEXOS_API ANexosColosseumGameModeBase : public AGameMode
{
	GENERATED_UCLASS_BODY()

		/** The bot pawn class */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameMode)
		TSubclassOf<APawn> BotPawnClass;

	UFUNCTION(exec)
		void SetAllowBots(bool bInAllowBots, int32 InMaxBots = 8);

		virtual void PreInitializeComponents() override;

	/** Initialize the game. This is called befor the actors' start */
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	/** Accept or regect a player who attempts to join the game */
	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;

	/** Start the match warmup */
	virtual void PostLogin(APlayerController* NewPlayer) override;

	/** Try to spawn the players pawn */
	virtual void RestartPlayer(AController* NewPlayer) override;

	/** Try to find the best spawn point for the player */
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	/** Always chose a random spot to spawn */
	virtual bool ShouldSpawnAtStartSpot(AController* Player) override;

	/** Returns default pawn class for a given controller */
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;

	/** prevents 'friendly fire' */
	virtual float ModifyDamage(float Damage, AActor* DamageActor, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) const;

	/** notify about kills in game */
	virtual void Killed(AController* Killer, AController* KilledPlayer, APawn* KilledPawn, const UDamageType* DamageType);

	/** can players damage each other */
	virtual bool CanDealDamage(ANexosPlayerState* DamageInstigator, ANexosPlayerState* DamagedPlayer) const;

	virtual bool AllowCheats(APlayerController* P) override;

	/** Update remaining time */
	virtual void DefaultTimer();

	/** called before startmatch */
	UFUNCTION(BlueprintCallable)
	virtual void HandleMatchIsWaitingToStart() override;

	/** starts new match */
	UFUNCTION(BlueprintCallable)
	virtual void HandleMatchHasStarted() override;

	/** new player joins */
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

	/** hides the onscreen hud and restarts the map */
	virtual void RestartGame() override;

	/** Creates AIControllers for all bots */
	void CreateBotControllers();

	/** Create a bot */
	AShooterAIController* CreateBot(int32 BotNum);

	virtual void PostInitProperties() override;

protected:

	/** delay between the first player join and match start */
	UPROPERTY(config)
		int32 WarmupTime;

	/** match duration */
	UPROPERTY(config)
		int32 RoundTime;

	/** How long before we got back to lobby*/
	UPROPERTY(config)
		int32 TimeBetweenGames;

	/** score for kills */
	UPROPERTY(config)
		int32 KillScore;

	/** score for death */
	UPROPERTY(config)
		int32 DeathScore;

	/** scale for self damage */
	UPROPERTY(config)
		float DamageSelfScale;

	UPROPERTY(config, BlueprintReadWrite)
		int32 MaxBots;

	UPROPERTY()
		TArray<AShooterAIController*> BotControllers;

	UPROPERTY(config)
		TSubclassOf<ANexosPlayerController> PlatformPlayerControllerClass;

	/** Handle for clean managment of DefaultTimer timer */
	FTimerHandle TimerHandle_DefaultTimer;

	bool bNeedsBotCreation;

	bool bAllowBots;

	/** spawning all bots for this game */
	void StartBots();

	/** initialization for bot after creation */
	virtual void InitBot(AShooterAIController* AIC, int32 BotNum);

	/** check who won the match */
	virtual void DetermineMatchWinner();

	/** check if the player state is a winner */
	virtual bool IsWinner(ANexosPlayerState* PlayerState) const;

	/** Can a player use this spawn point? */
	virtual bool IsSpawnpointAllowed(APlayerStart* SpawnPoint, AController* Player) const;

	/** shoul a player use this spawn pont? */
	virtual bool IsSpawnpointPreferred(APlayerStart* SpawnPoint, AController* Player) const;

	/** Returnd the game session class to use */
	virtual TSubclassOf<AGameSession> GetGameSessionClass() const override;

public:

	/** finish current match and lock players */
	UFUNCTION(exec)
		void FinishMatch();

	/*Finishes the match and bumps everyone to main menu.*/
	/*Only GameInstance should call this function */
	void RequestFinishAndExitToMainMenu();

	/** get the name of the bots count option used in server travel URL */
	static FString GetBotsCountOptionName();

};
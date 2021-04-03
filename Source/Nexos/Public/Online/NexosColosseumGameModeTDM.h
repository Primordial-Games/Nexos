// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NexosTeamStart.h"
#include "Online/NexosColosseumGameModeBase.h"
#include "NexosColosseumGameModeTDM.generated.h"

class ANexosPlayerState;

/**
 *
 */
UCLASS()
class NEXOS_API ANexosColosseumGameModeTDM : public ANexosColosseumGameModeBase
{
	GENERATED_UCLASS_BODY()

		/** setup team changes at player login */
		void PostLogin(APlayerController* NewPlayer) override;

	/** initialize replicated game data */
	virtual void InitGameState() override;

	/** can players damage each other? */
	virtual bool CanDealDamage(ANexosPlayerState* DamageInstigator, ANexosPlayerState* DamagedPlayer) const override;

protected:

	/** number of teams */
	int32 NumTeams;

	/** best team */
	int32 WinnerTeam;

	/** pick team with least players in or random when it's equal */
	int32 ChooseTeam(ANexosPlayerState* ForPlayerState) const;

	/** check who won */
	virtual void DetermineMatchWinner() override;

	/** check if PlayerState is a winner */
	virtual bool IsWinner(ANexosPlayerState* PlayerState) const override;

	/** check team constraints */
	virtual bool IsSpawnpointAllowed(APlayerStart* SpawnPoint, AController* Player) const;

	virtual void InitBot(AShooterAIController* AIC, int32 BotNum) override;
};

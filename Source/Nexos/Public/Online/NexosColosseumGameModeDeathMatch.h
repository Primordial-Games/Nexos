// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Online/NexosColosseumGameModeBase.h"
#include "NexosColosseumGameModeDeathMatch.generated.h"

class ANexosPlayerState;

/**
 *
 */
UCLASS()
class NEXOS_API ANexosColosseumGameModeDeathMatch : public ANexosColosseumGameModeBase
{
	GENERATED_UCLASS_BODY()

protected:

	/** best player */
	UPROPERTY(transient)
		ANexosPlayerState* WinnerPlayerState;

	/** check who won */
	virtual void DetermineMatchWinner() override;

	/** check if PlayerState is a winner */
	virtual bool IsWinner(ANexosPlayerState* PlayerState) const override;

};

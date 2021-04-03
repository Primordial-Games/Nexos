// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NexosPlayerState.h"
#include "GameFramework/GameState.h"
#include "NexosColosseumGameState.generated.h"

/** ranked PlayerState map, created from GameState */
typedef TMap<int32, TWeakObjectPtr<ANexosPlayerState> > RankedPlayerMap;

UCLASS()
class NEXOS_API ANexosColosseumGameState : public AGameState
{
	GENERATED_UCLASS_BODY()

public:

	/** number of teams in current game (doesn't deprecate when no players are left in a team) */
	UPROPERTY(Transient, Replicated)
		int32 NumTeams;

	/** accumulated score per team */
	UPROPERTY(Transient, Replicated)
		TArray<int32> TeamScores;

	/** time left for warmup / match */
	UPROPERTY(Transient, BlueprintReadOnly, Replicated)
		int32 RemainingTime;

	/** is timer paused? */
	UPROPERTY(Transient, Replicated)
		bool bTimerPaused;

	/** gets ranked PlayerState map for specific team */
	void GetRankedMap(int32 TeamIndex, RankedPlayerMap& OutRankedMap) const;

	void RequestFinishAndExitToMainMenu();


};

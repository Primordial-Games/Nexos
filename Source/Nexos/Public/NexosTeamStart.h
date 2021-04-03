// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "NexosTeamStart.generated.h"

/**
 *
 */
UCLASS()
class NEXOS_API ANexosTeamStart : public APlayerStart
{
	GENERATED_UCLASS_BODY()

	/** Which team can start at this point */
	UPROPERTY(EditInstanceOnly, Category = Team)
		int32 SpawnTeam;

	/** Whether players can start at this point */
	UPROPERTY(EditInstanceOnly, Category = Team)
		uint32 bNotForPlayers : 1;

	/** Whether bots can start at this point */
	UPROPERTY(EditInstanceOnly, Category = Team)
		uint32 bNotForBots : 1;

};

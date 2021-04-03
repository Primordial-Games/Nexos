// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "PlayFabJsonObject.h"
#include "GameFramework/PlayerState.h"
#include "ColosseumPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class NEXOS_API AColosseumPlayerState : public APlayerState
{
	GENERATED_UCLASS_BODY()

	
	/*
    *Key elements needed for the player to make calls and be identified
    */
	UPROPERTY(BlueprintReadWrite)
	FString PlatformId;

	UPROPERTY(BlueprintReadWrite)
	FString PlayFabId;

	UPROPERTY(BlueprintReadWrite)
	UPlayFabJsonObject* EntityKey;

	UPROPERTY(BlueprintReadWrite)
	UPlayFabJsonObject* Attributes;
	


};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "PlayFabJsonObject.h"
#include "Blueprint/UserWidget.h"
#include "CharacterSelectWidget.generated.h"

/**
 * 
 */
UCLASS()
class NEXOS_API UCharacterSelectWidget : public UUserWidget
{
	GENERATED_UCLASS_BODY()
	
	UPROPERTY(BlueprintReadWrite, Category = PlayerData)
		TArray<UPlayFabJsonObject*> Characters;

};

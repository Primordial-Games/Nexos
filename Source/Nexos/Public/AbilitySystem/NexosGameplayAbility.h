// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Nexos.h"
#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "NexosGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class NEXOS_API UNexosGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	
	UNexosGameplayAbility();

	// Abilities with this set will automatically activate when the input is pressed
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ability")
		ENexosAbilityInputID AbilityInputID = ENexosAbilityInputID::None;
};

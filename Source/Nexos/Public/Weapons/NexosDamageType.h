// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DamageType.h"
#include "Engine/Canvas.h"
#include "GameFramework/ForceFeedbackEffect.h"
#include "NexosDamageType.generated.h"

/**
 *
 */
UCLASS(const, Blueprintable, BlueprintType)
class UNexosDamageType : public UDamageType
{
	GENERATED_UCLASS_BODY()

		/** icon displayed in death messages log when killed with this weapon */
		UPROPERTY(EditDefaultsOnly, Category = HUD)
		FCanvasIcon KillIcon;

	/** force feedback effect to play on a player hit by this damage type */
	UPROPERTY(EditDefaultsOnly, Category = Effects)
		UForceFeedbackEffect* HitForceFeedback;

	/** force feedback effect to play on a player killed by this damage type */
	UPROPERTY(EditDefaultsOnly, Category = Effects)
		UForceFeedbackEffect* KilledForceFeedback;
};

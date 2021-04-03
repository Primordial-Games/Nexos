// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "NexosPlayerController_Menu.generated.h"

/**
 * 
 */
UCLASS()
class NEXOS_API ANexosPlayerController_Menu : public APlayerController
{
	GENERATED_UCLASS_BODY()

public:

	/** After game is initialized */
		virtual void PostInitializeComponents() override;

	/** setup inputs */
		virtual void SetupInputComponent() override;

	/** called when starting game */
		void StartGameFromTitle();
		
	/** See if we have already passed into the title */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Initialization)
		bool bIsInTitle;

	/** See if we are Initialized */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Initialization)
		bool Initialized;

		void ShowLoginMenu();
};

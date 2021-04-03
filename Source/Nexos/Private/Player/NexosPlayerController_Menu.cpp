// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/NexosPlayerController_Menu.h"
#include "WidgetController.h"
#include "NexosGameInstance.h"
#include "Nexos.h"

ANexosPlayerController_Menu::ANexosPlayerController_Menu(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void ANexosPlayerController_Menu::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	Initialized = false;
}

void ANexosPlayerController_Menu::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("StartGame", IE_Pressed, this, &ANexosPlayerController_Menu::StartGameFromTitle);
}

void ANexosPlayerController_Menu::StartGameFromTitle()
{
	if (bIsInTitle && !Initialized)
	{
		Initialized = true;

		UNexosGameInstance* GameInstance = Cast<UNexosGameInstance>(GetWorld()->GetGameInstance());

		GameInstance->ConnectToOnlineSystem();
	}
}

void ANexosPlayerController_Menu::ShowLoginMenu()
{
}

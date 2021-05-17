/*////////////////////////////////////////////////////////////////////
* Copyright (c) 2021 Primordial Games LLC
* All rights reserved.
*////////////////////////////////////////////////////////////////////


#include "Player/NexosPlayerControllerBase.h"

#include "NexosGameInstance.h"
#include "PlayFab.h"
#include "PlayFabClientDataModels.h"
#include "PlayFabDataDataModels.h"
#include "PlayFabMultiplayerDataModels.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Player/NexosPlayerCharacterBase.h"


ANexosPlayerControllerBase::ANexosPlayerControllerBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bStarChartPressed = false;
	bIsInCharacterMenu = false;
	bIsInStarChart = false;
}

void ANexosPlayerControllerBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	GetCharacterDataAndInventory();
}

void ANexosPlayerControllerBase::SetupInputComponent()
{
	Super::SetupInputComponent();

	//Player and Global Menus
	InputComponent->BindAction("CharacterMenu", IE_Pressed, this, &ANexosPlayerControllerBase::OpenCharacterMenu);
	InputComponent->BindAction("StarChart", IE_Pressed, this, &ANexosPlayerControllerBase::PressedStarChart);
	InputComponent->BindAction("StarChart", IE_Released, this, &ANexosPlayerControllerBase::ReleaseStarChart);

	InputComponent->BindAction("Gamemenu", IE_Pressed, this, &ANexosPlayerControllerBase::OpenGameMenu);

	//Voice chat for PTT
	InputComponent->BindAction("PushToTalk", IE_Pressed, this, &APlayerController::StartTalking);
	InputComponent->BindAction("PushToTalk", IE_Released, this, &APlayerController::StopTalking);

	//Toggle text chat
	InputComponent->BindAction("ToggleChat", IE_Pressed, this, &ANexosPlayerControllerBase::ToggleChatWindow);
}

void ANexosPlayerControllerBase::GetCharacterDataAndInventory()
{
	UNexosGameInstance* NexosGameInstance = Cast<UNexosGameInstance>(GetWorld()->GetGameInstance());
	//NexosGameInstance->GetPlayerCharacterObjects();	
}



void ANexosPlayerControllerBase::PopulateCharacterInventorySlots()
{
	
}

void ANexosPlayerControllerBase::OpenCharacterMenu()
{
	/*
	 *Character and Star Chart UI opening and closing
	 */
	//If we are in the Star Chart or Game Menu close it.
	if (bIsInStarChart)
	{
		bIsInStarChart = false;
		StarChartRef->RemoveFromParent();
	}
	else if (bIsInGameMenu)
	{
		bIsInGameMenu = false;
		GameMenuWidget->RemoveFromParent();
	}
	
	//If we are not in the character menu, open it
	if (!bIsInCharacterMenu)
	{
		bIsInCharacterMenu = true;
		//do we already have a valid Character menu?
		if (!CharacterMenuRef)
			CharacterMenuRef = CreateWidget<UUserWidget>(this, CharacterMenu);
		CharacterMenuRef->AddToViewport();
		CharacterMenuRef->SetFocus();
		SetShowMouseCursor(true);
		return;
	}

	//if we are in the character menu close it
	bIsInCharacterMenu = false;
	CharacterMenuRef->RemoveFromParent();
	SetShowMouseCursor(false);
}

void ANexosPlayerControllerBase::PressedStarChart()
{
	//If we are in the Character menu, just open the star chart and close the Character Menu.
	if (bIsInCharacterMenu)
	{
		bIsInCharacterMenu = false;
		CharacterMenuRef->RemoveFromParent();
		OpenStarChart();
		return;
	}
	else if (bIsInGameMenu)
	{
		bIsInGameMenu = false;
		GameMenuWidget->RemoveFromParent();
		OpenStarChart();
		return;
	}
	//If the Star Chart is already open them close it.
	if (bIsInStarChart)
	{
		bIsInStarChart = false;
		StarChartRef->RemoveFromParent();
		SetShowMouseCursor(false);
		return;
	}

	GetWorld()->GetTimerManager().SetTimer(StarChartTimer, this, &ANexosPlayerControllerBase::OpenStarChart, .5, false);
	if (!StarChartIconRef)
		StarChartIconRef = CreateWidget<UUserWidget>(this, StarChartIcon);
	StarChartIconRef->AddToViewport();
}

void ANexosPlayerControllerBase::ReleaseStarChart()
{
	GetWorld()->GetTimerManager().ClearTimer(StarChartTimer);
	StarChartIconRef->RemoveFromParent();
}

void ANexosPlayerControllerBase::OpenStarChart()
{

	StarChartIconRef->RemoveFromParent();
	
	bIsInStarChart = true;
	//do we already have a valid Star Chart?
	if(!StarChartRef)
		StarChartRef = CreateWidget<UUserWidget>(this, StarChart);
	StarChartRef->AddToViewport();
	StarChartRef->SetFocus();
	SetShowMouseCursor(true);
	
}

void ANexosPlayerControllerBase::OpenGameMenu()
{
	//If we are in the Character menu or Star Chart, close it.
	if (bIsInCharacterMenu)
	{
		bIsInCharacterMenu = false;
		CharacterMenuRef->RemoveFromParent();
	}
	else if (bIsInStarChart)
	{
		bIsInStarChart = false;
		StarChartRef->RemoveFromParent();
	}

	if (!bIsInGameMenu)
	{
		bIsInGameMenu = true;
		//do we already have a valid Character menu?
		if (!GameMenuWidget)
			GameMenuWidget = CreateWidget<UUserWidget>(this, GameMenuClass);
		GameMenuWidget->AddToViewport();
		GameMenuWidget->SetFocus();
		SetShowMouseCursor(true);
		return;
	}

	//if we are in the game menu, close it.
	bIsInGameMenu = false;
	GameMenuWidget->RemoveFromParent();
	SetShowMouseCursor(false);
}

void ANexosPlayerControllerBase::ToggleChatWindow()
{

}

void ANexosPlayerControllerBase::CreateInventoryItems(UPlayFabJsonObject* Inventroy)
{
	if (GetLocalRole() < ROLE_Authority)
	{
		Server_CreateInventoryItems(Inventroy);
		return;
	}
    
}

bool ANexosPlayerControllerBase::Server_CreateInventoryItems_Validate(UPlayFabJsonObject* Inventroy)
{
	return true;
}

void ANexosPlayerControllerBase::Server_CreateInventoryItems_Implementation(UPlayFabJsonObject* Inventroy)
{
	CreateInventoryItems(Inventroy);
}

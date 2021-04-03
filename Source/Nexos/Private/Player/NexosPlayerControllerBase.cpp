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
	//If we are not in the character menu, open it
	if (!bIsInCharacterMenu)
	{
		bIsInCharacterMenu = true;
		//do we already have a valid Character menu?
		if (!CharacterMenuRef)
			CharacterMenuRef = CreateWidget<UUserWidget>(this, CharacterMenu);
		CharacterMenuRef->AddToViewport();
		return;
	}

	//If we are in teh Star Chart close it.
	if (bIsInStarChart)
	{
		bIsInStarChart = false;
		StarChartRef->RemoveFromParent();

		bIsInCharacterMenu = true;
		//do we already have a valid Character menu?
		if (!CharacterMenuRef)
			CharacterMenuRef = CreateWidget<UUserWidget>(this, CharacterMenu);
		CharacterMenuRef->AddToViewport();
		return;
	}

	//if we are in the caracter menu close it
	bIsInCharacterMenu = false;
	CharacterMenuRef->RemoveFromParent();
}

void ANexosPlayerControllerBase::PressedStarChart()
{
	//If we are in teh Character menu, just open the star chart and close the Character Menu.
	if (bIsInCharacterMenu)
	{
		bIsInCharacterMenu = false;
		CharacterMenuRef->RemoveFromParent();
		OpenStarChart();
		return;
	}
	
	//If the Star Chart is already open them close it.
	if (bIsInStarChart)
	{
		bIsInStarChart = false;
		StarChartRef->RemoveFromParent();
		return;
	}

	GetWorld()->GetTimerManager().SetTimer(StarChartTimer, this, &ANexosPlayerControllerBase::OpenStarChart, .5, false);
}

void ANexosPlayerControllerBase::ReleaseStarChart()
{
	GetWorld()->GetTimerManager().ClearTimer(StarChartTimer);
}

void ANexosPlayerControllerBase::OpenStarChart()
{
	bIsInStarChart = true;
	//do we already have a valid Star Chart?
	if(!StarChartRef)
		StarChartRef = CreateWidget<UUserWidget>(this, StarChart);
	StarChartRef->AddToViewport();
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

/*
* Copyright (c) 2021 Primordial Games LLC
* All rights reserved.
*/

#pragma once

#include "CoreMinimal.h"

#include "OnlineSubsystem.h"
#include "PlayFabAuthenticationContext.h"
#include "PlayFabJsonObject.h"
#include "GameFramework/PlayerController.h"
#include "NexosPlayerControllerBase.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class NEXOS_API ANexosPlayerControllerBase : public APlayerController
{
	GENERATED_UCLASS_BODY()


	virtual void PostInitializeComponents() override;
	
	/** sets up input */
	virtual void SetupInputComponent() override;

	/**
	 *Get Information from PlayFab
	*/

	UFUNCTION()
		void GetCharacterDataAndInventory();
	
	UFUNCTION()
		void PopulateCharacterInventorySlots();

	/**
	 *Menu Inputs for all Game Types
	 **/
	UFUNCTION()
		void OpenCharacterMenu();

	UFUNCTION()
		void OpenStarChart();

	UFUNCTION()
		void PressedStarChart();

	UFUNCTION()
		void ReleaseStarChart();

	UFUNCTION()
		void ToggleChatWindow();


	/**
	 *All the different UI that a player can pull up
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Player UI")
		TSubclassOf<class UUserWidget> StarChart;

	UPROPERTY(BlueprintReadWrite, Category = UI)
		UUserWidget* StarChartRef;
	
	UPROPERTY(EditDefaultsOnly, Category = "Player UI")
		TSubclassOf<class UUserWidget> CharacterMenu;

	UPROPERTY(BlueprintReadWrite, Category = UI)
		UUserWidget* CharacterMenuRef;

	UPROPERTY(BlueprintReadWrite, Category = UI)
		bool bIsInCharacterMenu;

	UPROPERTY(BlueprintReadWrite, Category = UI)
		bool bIsInStarChart;

	UPROPERTY(BlueprintReadWrite, Category = PlayFab)
		UPlayFabJsonObject* EntityKey;

	UPROPERTY(BlueprintReadWrite, Category = PlayFab)
		UPlayFabAuthenticationContext* AuthKey;
	
	UPROPERTY(BlueprintReadWrite, Category = PlayFab)
	    FString Username;
	    
	bool bStarChartPressed;

	FTimerHandle StarChartTimer;

	IOnlineSubsystem* OnlineInterface = IOnlineSubsystem::Get();

	/* Player Weapon Loadout
	* The Controller pulls the data from PlayFab, the GameMode Decodes the data and stores it here
	**/
	UPROPERTY(EditDefaultsOnly, Category = Inventory)
	TMap<FString, TSubclassOf<class ANexosWeaponBase> > PlayerEquippedWeapons;

	/* Player Armor Loadout
	* The Controller pulls the data from PlayFab, the GameMode Decodes the data and stores it here
	**/
	UPROPERTY(EditDefaultsOnly, Category = Inventory)
	TMap<FString, TSubclassOf<class ANexosArmorBase> > PlayerEquippedArmor;
	
	/* Players Weapon Inventory, this should be pulled from PlayFab upon controller creation
	* The Controller pulls the data from PlayFab, the GameMode Decodes the data and stores it here
	* Currently pulls from Stock Data
	*/
	UPROPERTY(EditDefaultsOnly, Category = Inventory)
		TArray<TSubclassOf<class ANexosWeaponBase> > PrimaryWeaponInventory;
	UPROPERTY(EditDefaultsOnly, Category = Inventory)
		TArray<TSubclassOf<class ANexosWeaponBase> > SpecialWeaponInventory;
	UPROPERTY(EditDefaultsOnly, Category = Inventory)
		TArray<TSubclassOf<class ANexosWeaponBase> > HeavyWeaponInventory;
	UPROPERTY(EditDefaultsOnly, Category = Inventory)
		TArray<TSubclassOf<class ANexosWeaponBase> > SidearmWeaponInventory;


	/* Players Armor Inventory, this should be pulled from PlayFab upon controller creation
	* The Controller pulls the data from PlayFab, the GameMode Decodes the data and stores it here
	* Currently pulls from Stock Data
	*/
	UPROPERTY(EditDefaultsOnly, Category = Inventory)
		TArray<TSubclassOf<class ANexosArmorBase> > HelmetArmorInventory;
	UPROPERTY(EditDefaultsOnly, Category = Inventory)
		TArray<TSubclassOf<class ANexosArmorBase> > ArmsArmorInventory;
	UPROPERTY(EditDefaultsOnly, Category = Inventory)
		TArray<TSubclassOf<class ANexosArmorBase> > TorsoArmorInventory;
	UPROPERTY(EditDefaultsOnly, Category = Inventory)
		TArray<TSubclassOf<class ANexosArmorBase> > LegArmorInventory;
	UPROPERTY(EditDefaultsOnly, Category = Inventory)
		TArray<TSubclassOf<class ANexosArmorBase> > ClassOrnamentInventory;

	UFUNCTION(BlueprintCallable, Category = Inventory)
	void CreateInventoryItems(UPlayFabJsonObject* Inventroy);

	UFUNCTION(reliable, server, WithValidation)
    void Server_CreateInventoryItems(UPlayFabJsonObject* Inventroy);
	
};
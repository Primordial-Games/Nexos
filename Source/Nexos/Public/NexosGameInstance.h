/*////////////////////////////////////////////////////////////////////
*Copyright (c) 2021 Primordial Games LLC
* All rights reserved. 
*////////////////////////////////////////////////////////////////////

#pragma once

#include "CoreMinimal.h"
#include "PlayFab.h"
#include "Core/PlayFabError.h"
#include "Core/PlayFabClientDataModels.h"
#include "Core/PlayFabMultiplayerDataModels.h"
#include "PlayFabJsonObject.h"
#include "PlayFabJsonValue.h"
#include "Online.h"
#include "UI/CharacterSelectWidget.h"

#include "NexosGameInstance.generated.h"

/**
 * 
 */

class FVariantData;
class FNexosTitleMenu;
class FNexosMainMenu;
class FNexosMessageMenu;
class ANexosGameSession;

namespace NexosGameInstanceState
{
	extern const FName None;
	extern const FName PendingInvite;
	extern const FName TitleScreen;
	//extern const FName TitleMenu;
	extern const FName LobbyMenu;
	extern const FName MessageMenu;
	extern const FName Playing;
}

UENUM()
enum class EOnlineMode : uint8
{
	Offline,
	Online
};


UCLASS(config = Game)
class NEXOS_API UNexosGameInstance : public UGameInstance
{
public:

	GENERATED_UCLASS_BODY()

public:

	virtual void Init() override;
	virtual void Shutdown() override;
	
	UFUNCTION(BlueprintNativeEvent, Category = Settings)
	void InitSettings();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tooltips")
		FString LoadingMessage;

	UPROPERTY(BlueprintReadWrite, Category = "PlayFab")
		FString PlayFabID;

	UPROPERTY(BlueprintReadWrite, Category = "PlayFab")
		FString PlatformId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayFab")
		FString Username;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayFab")
		FString CharacterId;

/**
 *PlayFab systems and connections
*/
	UFUNCTION(BlueprintCallable, Category = "Online Services")
		void ConnectToOnlineSystem();

	void OpenLoginMenu(FString ErrorMessage);
	
	UFUNCTION(BlueprintCallable)
        void LoginToPlayFab();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Online Services")
        void GetPlayerLatancy();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Online Services")
		void GrantCharacterTokenToPlayer();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Online Services")
		void PurgePlayerCharacters();
	
	UFUNCTION(BlueprintCallable, Category = "Online Services")
        void SelectCharacter();

	UFUNCTION(BlueprintCallable, Category = "Omline Services")
		void CreateCharacter();

	UFUNCTION(BlueprintCallable, Category = "Omline Services")
		void GetPlayFabUsername();

	UFUNCTION(BlueprintCallable, Category = "Online Services")
		void OpenMainLobby();

	UFUNCTION(BlueprintNativeEvent)
		void GetPlayerCharacterObjects();

	void SetSaveData();

	UFUNCTION(BlueprintNativeEvent, Category = SaveData)
	void GetSteamName();
	
	//Callbacks for PlayFabAPI
		//Login Callbacks
		void OnLoginSuccess(const PlayFab::ClientModels::FLoginResult& Result);
		void OnLoginError(const PlayFab::FPlayFabCppError& ErrorResult);

		//General Callbacks
		void OnPlayFabError(const PlayFab::FPlayFabCppError& ErrorResult);
		void OnGetCharactersSuccess(const PlayFab::ClientModels::FListUsersCharactersResult& Result);
		void OnGetUsernameSuccess(const PlayFab::ClientModels::FGetAccountInfoResult& Result);
		void OnUpdateCharacterDataSuccess(const PlayFab::ClientModels::FUpdateCharacterDataResult& Result);
		void OnUpdatePlayerDataSuccess(const PlayFab::ClientModels::FUpdateUserDataResult& Result);
		void OnGetCharacterInventory(const PlayFab::ClientModels::FGetCharacterInventoryResult& Result);
	
	void SaveGameDelegateFunction(const FString& SlotName, const int32 UserIndex, bool bSuccess);

/**
 *General Gameplay systems
 *-Errors-Loading Screens-Messages
 **/
	UFUNCTION()
		void ShowDefaultLoadingScreen();

	IOnlineSubsystem* OnlineInterface = IOnlineSubsystem::Get();

	void GotoState(FName NewState);
	void ShowMessageThenGotoState(const FText& Message, const FText& OKButtonString, const FText& CanelButtonString, const FName& NewState, const bool OverrideExisting = true, TWeakObjectPtr<ULocalPlayer>PlayerOwner = nullptr);

	void CleanupSessionOnReturnToMenu();

	/** Returns true if the game is in online mode */
	EOnlineMode GetOnlineMode() const { return OnlineMode; }

	/** Sets the online mode of the game */
	void SetOnlineMode(EOnlineMode InOnlineMode);

	const FName GetCurrentState() const;

	UPROPERTY(config)
		FString TitleId;

/**
 *UI Elements
 **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LoginUI)
		TSubclassOf<class UUserWidget> LoginMenu;
	UPROPERTY(BlueprintReadWrite, Category = LoginUI)
		UUserWidget* LoginMenuRef;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LoadingScreen)
		TSubclassOf<class UUserWidget> LoadingScreenBase;
	UPROPERTY(BlueprintReadWrite, Category = LoadingScreen)
		UUserWidget* LoadingScreenBaseRef;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LoadingScreen)
		TSubclassOf<class UCharacterSelectWidget> CharacterSelectScreen;
	UPROPERTY(BlueprintReadWrite, Category = LoadingScreen)
		UCharacterSelectWidget* CharacterSelectScreenRef;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LoadingScreen)
		TSubclassOf<class UUserWidget> CharacterCreateScreen;
	UPROPERTY(BlueprintReadWrite, Category = LoadingScreen)
		UUserWidget* CharacterCreateScreenRef;

	//The Id and Type for PlayFab, used as the Entity for interacting with account 
	UPROPERTY(BlueprintReadWrite, Category = PlayFab)
		UPlayFabJsonObject* EntityKey;

	//Latency for the players connection to all avalable servers.
	UPROPERTY(BlueprintReadWrite, Category = PlayFab)
		UPlayFabJsonObject* latencies;

	TArray<PlayFab::ClientModels::FItemInstance> Inventory;
	
private:

	EOnlineMode OnlineMode;

	FName CurrentState;

	USoundCue* LoadingSound;

private:
	bool bInitialized;
	bool bInitializing;

	bool bLoggedIn;
	bool bLoggingIn;

	PlayFabClientPtr ClientAPI = nullptr;
	PlayFabMultiplayerPtr MultiplayerAPI = nullptr;
};


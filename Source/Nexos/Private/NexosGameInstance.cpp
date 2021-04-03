/*////////////////////////////////////////////////////////////////////
*Copyright (c) 2021 Primordial Games LLC
* All rights reserved. 
*////////////////////////////////////////////////////////////////////

#include "NexosGameInstance.h"
#include "OnlineSubsystem.h"
#include "PlayFabClientDataModels.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Core/PlayFabClientAPI.h"
#include "Core/PlayFabMultiplayerAPI.h"
#include "GameFramework/PlayerState.h"
#include "Online/NexosColosseumGameState.h"
#include "Player/NexosPersistentUser.h"
#include "Player/NexosPlayerController_Menu.h"
#include "Widgets/Text/ISlateEditableTextWidget.h"


namespace NexosColosseumGameState
{
	const FName None = FName(TEXT("None"));
	const FName PendingInvite = FName(TEXT("PendingInvite"));
	const FName WelcomeScreen = FName(TEXT("TitleScreen"));
	//const FName TitleMenu = FName(TEXT("TitleMenu"));
	const FName LobbyMenu = FName(TEXT("LobbyMenu"));
	const FName MessageMenu = FName(TEXT("MessageMenu"));
	const FName Playing = FName(TEXT("Playing"));
}

UNexosGameInstance::UNexosGameInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, OnlineMode(EOnlineMode::Online) // Default to online
	//, bIsLicensed(true) // Default to licensed (should have been checked by OS on boot)
{

	static ConstructorHelpers::FObjectFinder<USoundCue> LoadingCue(TEXT("/Game/Sound/Sci-fi_Evolution/Robotics_and_Tech/UI_Button_with_Sub_LFE_CUE"));
	if (LoadingCue.Object != nullptr)
		LoadingSound = LoadingCue.Object;
}

void UNexosGameInstance::Init()
{
	Super::Init();
	InitSettings();
}

//Shut down all systems and connections withing the title
void UNexosGameInstance::Shutdown()
{
	Super::Shutdown();

#if UE_SERVER
	//Incase we are a server tell the server to shutdown
	FGenericPlatformMisc::RequestExit(false);
#endif

}

/*
Login function for PlayFab, the game checks wat platform it is on then uses the correct Login System
Need to improve Steam system for other store-fronts
*/
void UNexosGameInstance::LoginToPlayFab()
{
#if PLATFORM_PS4
	//PlayStation Login

#elif PLATFORM_XBOXONE
	//Xbox Login
	PlayFab::ClientModels::FLoginWithXboxRequest request;


#else
	//Steam Login
	if (IOnlineSubsystem::Get(FName("Steam")))
	{
		GetSteamName();
		LoadingMessage = "Connecting with Steam";
		PlayFab::ClientModels::FLoginWithSteamRequest request;
		request.TitleId = "DB79E";
		request.CreateAccount = true;
		request.SteamTicket = OnlineInterface->GetIdentityInterface()->GetAuthToken(0);

		ClientAPI->LoginWithSteam(request,
			PlayFab::UPlayFabClientAPI::FLoginWithSteamDelegate::CreateUObject(this, &UNexosGameInstance::OnLoginSuccess),
			PlayFab::FPlayFabErrorDelegate::CreateUObject(this, &UNexosGameInstance::OnLoginError));
	}
	//Other systems that requires a PlayFab login
	else
	{
		OpenLoginMenu("Failed to connect to Steam");
	}
#endif
}

/*
*If we are able to login to PlayFab then we should get the needed information
*/
void UNexosGameInstance::OnLoginSuccess(const PlayFab::ClientModels::FLoginResult& Result)
{
	UE_LOG(LogTemp, Error, TEXT("Nexos - PlayFab Callback: Player was logged in"));
	PlayFabID = Result.PlayFabId;
	EntityKey = NewObject<UPlayFabJsonObject>();
	EntityKey->SetStringField("id", Result.EntityToken->Entity->Id);
	EntityKey->SetStringField("type", Result.EntityToken->Entity->Type);

	if (Result.NewlyCreated)
	{
		UE_LOG(LogTemp, Error, TEXT("Nexos - PlayFab Callback: New Player Detected, creading Login UI"));
		OpenLoginMenu("New Player Detected");
		return;
	}
	else
		GetPlayerLatancy();
	const FDateTime LastPlayerDateTimeUpdate = FDateTime(2021,1,15, 6);
	if (Result.LastLoginTime.mValue < LastPlayerDateTimeUpdate)
		PurgePlayerCharacters();
}


//If there is and error with logging in, log it and open the Login UI
void UNexosGameInstance::OnLoginError(const PlayFab::FPlayFabCppError& ErrorResult)
{
	UE_LOG(LogTemp, Error, TEXT("Failed to login via Steam.\nHere's some debug information:\n%s"), *ErrorResult.GenerateErrorReport());
}

//If there is a general PlayFab Error, log it and let the game continue if possible
void UNexosGameInstance::OnPlayFabError(const PlayFab::FPlayFabCppError& ErrorResult) 
{
	UE_LOG(LogTemp, Error, TEXT("Failed to login via Steam.\nHere's some debug information:\n%s"), *ErrorResult.GenerateErrorReport());
}

/*
*Start up the connection to PlayFab and any other core online system
*/
void UNexosGameInstance::ConnectToOnlineSystem()
{
	UWidgetLayoutLibrary::RemoveAllWidgets(this);

	//Init the API interaces so they can do the voodoo they doo
	ClientAPI = IPlayFabModuleInterface::Get().GetClientAPI();
	MultiplayerAPI = IPlayFabModuleInterface::Get().GetMultiplayerAPI();

	ShowDefaultLoadingScreen();

	LoginToPlayFab();
}

/*
 *Open the Login Menu for the player to login/Create a PlayFab Account
 */
void UNexosGameInstance::OpenLoginMenu(FString ErrorMessage)
{
	//Get the message to display in the loading screen
	if (ErrorMessage == "")
		if (PlayFabID == "")
			LoadingMessage = "Failed to login";
		else
			LoadingMessage = "New User Detected";
	else
		LoadingMessage = ErrorMessage;

	ANexosPlayerController_Menu* PC = Cast<ANexosPlayerController_Menu>(UGameplayStatics::GetPlayerController(this, 0));
	PC->bShowMouseCursor = true;

	//Create the Login Menu
	if (!LoginMenuRef)
		LoginMenuRef = CreateWidget<UUserWidget>(this, LoginMenu);
	LoginMenuRef->AddToViewport();
}

/*
 *Make the call to get all available player characters
 */
void UNexosGameInstance::SelectCharacter()
{
	PlayFab::ClientModels::FListUsersCharactersRequest Request;
	Request.PlayFabId = PlayFabID;

	ClientAPI->GetAllUsersCharacters(Request,
		PlayFab::UPlayFabClientAPI::FGetAllUsersCharactersDelegate::CreateUObject(this, &UNexosGameInstance::OnGetCharactersSuccess),
		PlayFab::FPlayFabErrorDelegate::CreateUObject(this, &UNexosGameInstance::OnPlayFabError));
}

/*
*Once we get the characters, display the characters on the playes screen
*/
void UNexosGameInstance::OnGetCharactersSuccess(const PlayFab::ClientModels::FListUsersCharactersResult& Result)
{
	ANexosPlayerController_Menu* PC = Cast<ANexosPlayerController_Menu>(UGameplayStatics::GetPlayerController(this, 0));
	PC->bShowMouseCursor = true;
	
	if (Result.Characters.Num() > 0)
	{
		if(!CharacterSelectScreenRef)
			CharacterSelectScreenRef = CreateWidget<UCharacterSelectWidget>(this, CharacterSelectScreen);
		for (PlayFab::ClientModels::FCharacterResult Character : Result.Characters)
		{
			UPlayFabJsonObject* CharacterResult = NewObject<UPlayFabJsonObject>();
			CharacterResult->SetStringField("CharacterId", Character.CharacterId);
			CharacterResult->SetStringField("CharacterName", Character.CharacterName);
			CharacterResult->SetStringField("CharacterType", Character.CharacterType);
			CharacterSelectScreenRef->Characters.Add(CharacterResult); 
		} 
		CharacterSelectScreenRef->AddToViewport();
	}
	else
	{
		CreateCharacter();
	}
}

/*
 *Create the Character Create Widget for players
 */
void UNexosGameInstance::CreateCharacter()
{
	if(!CharacterCreateScreenRef)
		CharacterCreateScreenRef = CreateWidget<UUserWidget>(this, CharacterCreateScreen);
	CharacterCreateScreenRef->AddToViewport();
}

/*
 *Get the Player Username from PlayFab if there is none from the Online System
 */
void UNexosGameInstance::GetPlayFabUsername()
{
	PlayFab::ClientModels::FGetAccountInfoRequest Request;
	Request.PlayFabId = PlayFabID;

	ClientAPI->GetAccountInfo(Request,
		PlayFab::UPlayFabClientAPI::FGetAccountInfoDelegate::CreateUObject(this, &UNexosGameInstance::OnGetUsernameSuccess),
		PlayFab::FPlayFabErrorDelegate::CreateUObject(this, &UNexosGameInstance::OnPlayFabError));
}

void UNexosGameInstance::OnGetUsernameSuccess(const PlayFab::ClientModels::FGetAccountInfoResult& Result)
{
	Username = Result.AccountInfo->Username;
	GetPlayerLatancy();
}


/*
 *Open the main lobby for the player and start gameplay
 */
void UNexosGameInstance::OpenMainLobby()
{
	ShowDefaultLoadingScreen();
	SetSaveData();
	UGameplayStatics::OpenLevel(this, "/Game/Maps/Menus/MultiplayerPrivateLobby");
}

/*
 *Get the save data and override it with the latest information from PlayFab
 */
void UNexosGameInstance::SetSaveData()
{
#if PLATFORM_PS4
		//PlayStation Save Data

#elif PLATFORM_XBOXONE
		//Xbox Save Data
#else
		//Steam Save Data
	if (IOnlineSubsystem::Get(FName("Steam")))
	{
		if(UNexosPersistentUser* LoadGame = Cast<UNexosPersistentUser>(UGameplayStatics::LoadGameFromSlot(OnlineInterface->GetIdentityInterface()->GetAuthToken(0), 0)))
		{
			
		}

		if(UNexosPersistentUser* SaveGameInstance = Cast<UNexosPersistentUser>(UGameplayStatics::CreateSaveGameObject(UNexosPersistentUser::StaticClass())))
		{
			FAsyncSaveGameToSlotDelegate SavedDelegate;
			// USomeUObjectClass::SaveGameDelegateFunction is a void function that takes the following parameters: const FString& SlotName, const int32 UserIndex, bool bSuccess
			SavedDelegate.BindUObject(this, &UNexosGameInstance::SaveGameDelegateFunction);
			SaveGameInstance->PlayFabId = PlayFabID;
			SaveGameInstance->EntityKey = EntityKey;
			SaveGameInstance->Atributes = latencies;
			SaveGameInstance->PlatformId = OnlineInterface->GetIdentityInterface()->GetAuthToken(0);
			SaveGameInstance->PlayerName = Username;
			UGameplayStatics::AsyncSaveGameToSlot(SaveGameInstance, PlatformId, 0, SavedDelegate);
		}

	}
	else //We will use our PlayFab Id as the save slot
	{
		if(UNexosPersistentUser* LoadGame = Cast<UNexosPersistentUser>(UGameplayStatics::LoadGameFromSlot(PlayFabID, 0)))
		{
			
		}
	
		if(UNexosPersistentUser* SaveGameInstance = Cast<UNexosPersistentUser>(UGameplayStatics::CreateSaveGameObject(UNexosPersistentUser::StaticClass())))
		{
			SaveGameInstance->PlayFabId = PlayFabID;
			SaveGameInstance->EntityKey = EntityKey;
			//todo platform ID wil be PlayFab ID and we need to get the username;
		}
	}
#endif
}


void UNexosGameInstance::SaveGameDelegateFunction(const FString& SlotName, const int32 UserIndex, bool bSuccess)
{
}

void UNexosGameInstance::ShowDefaultLoadingScreen()
{
	if (!LoadingScreenBaseRef)
		LoadingScreenBaseRef = CreateWidget<UUserWidget>(this, LoadingScreenBase);
	LoadingScreenBaseRef->AddToViewport();

	UGameplayStatics::PlaySound2D(UGameplayStatics::GetPlayerController(this,0), LoadingSound);
}



void UNexosGameInstance::GotoState(FName NewState)
{
}
void UNexosGameInstance::ShowMessageThenGotoState(const FText& Message, const FText& OKButtonString, const FText& CanelButtonString, const FName& NewState, const bool OverrideExisting, TWeakObjectPtr<ULocalPlayer> PlayerOwner)
{
}
void UNexosGameInstance::CleanupSessionOnReturnToMenu()
{
}
void UNexosGameInstance::SetOnlineMode(EOnlineMode InOnlineMode)
{
}




/*
 *Implementations for Blueprint Native Events
 */
void UNexosGameInstance::GetSteamName_Implementation()
{
}
void UNexosGameInstance::GetPlayerLatancy_Implementation()
{
}
void UNexosGameInstance::PurgePlayerCharacters_Implementation()
{
}
void UNexosGameInstance::InitSettings_Implementation()
{
}
void UNexosGameInstance::GrantCharacterTokenToPlayer_Implementation()
{
}
void UNexosGameInstance::GetPlayerCharacterObjects_Implementation()
{
}

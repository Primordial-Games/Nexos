// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/NexosPlayerController.h"
#include "Nexos.h"
#include "Player/NexosPlayerCameraManager.h"
#include "Player/NexosLocalPlayer.h"
#include "Player/NexosPersistentUser.h"
#include "Online/NexosPlayerState.h"
#include "Weapons/NexosWeaponBase.h"
#include "UI/NexosHUD.h"
#include "UI/Menu/NexosInGameMenu.h"
#include "UI/Style/NexosStyle.h"
#include "NexosGameInstance.h"
#include "NexosViewportClient.h"
#include "NexosLeaderboards.h"
#include "Sound/SoundNodeLocalPlayer.h"
#include "AudioThread.h"
#include "PlayFabClientDataModels.h"


#if !defined(TRACK_STATS_LOCALLY)
#define TRACK_STATS_LOCALLY 1
#endif

ANexosPlayerController::ANexosPlayerController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PlayerCameraManagerClass = ANexosPlayerCameraManager::StaticClass();
	bAllowGameActions = true;
	bGameEndedFrame = false;
	LastDeathLocation = FVector::ZeroVector;

	ServerSayString = TEXT("Say");
	NexosFriendUpdateTimer = 0.0f;
	bHasSentStartEvents = false;

	StatMatchesPlayed = 0;
	StatKills = 0;
	StatDeaths = 0;
	bHasFetchedPlatformData = false;
}

void ANexosPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// UI input
	InputComponent->BindAction("InGameMenu", IE_Pressed, this, &ANexosPlayerController::OnToggleInGameMenu);
	InputComponent->BindAction("Scoreboard", IE_Pressed, this, &ANexosPlayerController::OnShowScoreboard);
	InputComponent->BindAction("Scoreboard", IE_Released, this, &ANexosPlayerController::OnHideScoreboard);
	InputComponent->BindAction("ConditionalCloseScoreboard", IE_Pressed, this, &ANexosPlayerController::OnConditionalCloseScoreboard);
	InputComponent->BindAction("ToggleScoreboard", IE_Pressed, this, &ANexosPlayerController::OnToggleScoreboard);

	// voice chat
	InputComponent->BindAction("PushToTalk", IE_Pressed, this, &APlayerController::StartTalking);
	InputComponent->BindAction("PushToTalk", IE_Released, this, &APlayerController::StopTalking);

	InputComponent->BindAction("ToggleChat", IE_Pressed, this, &ANexosPlayerController::ToggleChatWindow);
}


void ANexosPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	FNexosStyle::Initialize();
	NexosFriendUpdateTimer = 0;
}

void ANexosPlayerController::ClearLeaderboardDelegate()
{

}

void ANexosPlayerController::TickActor(float DeltaTime, enum ELevelTick TickType, FActorTickFunction& ThisTickFunction)
{
	Super::TickActor(DeltaTime, TickType, ThisTickFunction);

	if (IsGameMenuVisible())
	{
		if (NexosFriendUpdateTimer > 0)
		{
			NexosFriendUpdateTimer -= DeltaTime;
		}
		else
		{
			TSharedPtr<class FNexosFriends> NexosFriends = NexosInGameMenu->GetNexosFriends();
			ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(Player);
			if (NexosFriends.IsValid() && LocalPlayer && LocalPlayer->GetControllerId() >= 0)
			{
				NexosFriends->UpdateFriends(LocalPlayer->GetControllerId());
			}

			// Make sure the time between calls is long enough that we won't trigger (0x80552C81) and not exceed the web api rate limit
			// That value is currently 75 requests / 15 minutes.
			NexosFriendUpdateTimer = 15;

		}
	}

	// Is this the first frame after the game has ended
	if (bGameEndedFrame)
	{
		bGameEndedFrame = false;

		// ONLY PUT CODE HERE WHICH YOU DON'T WANT TO BE DONE DUE TO HOST LOSS

		// Do we need to show the end of round scoreboard?
		if (IsPrimaryPlayer())
		{
			ANexosHUD* NexosHUD = GetNexosHUD();
			if (NexosHUD)
			{
				NexosHUD->ShowScoreboard(true, true);
			}
		}
	}

	const bool bLocallyControlled = IsLocalController();
	const uint32 UniqueID = GetUniqueID();
	FAudioThread::RunCommandOnAudioThread([UniqueID, bLocallyControlled]()
		{
			USoundNodeLocalPlayer::GetLocallyControlledActorCache().Add(UniqueID, bLocallyControlled);
		});
};

void ANexosPlayerController::BeginDestroy()
{
	Super::BeginDestroy();
	ClearLeaderboardDelegate();

	// clear any online subsystem references
	NexosInGameMenu = nullptr;

	if (!GExitPurge)
	{
		const uint32 UniqueID = GetUniqueID();
		FAudioThread::RunCommandOnAudioThread([UniqueID]()
			{
				USoundNodeLocalPlayer::GetLocallyControlledActorCache().Remove(UniqueID);
			});
	}
}

void ANexosPlayerController::SetPlayer(UPlayer* InPlayer)
{
	Super::SetPlayer(InPlayer);

	if (ULocalPlayer* const LocalPlayer = Cast<ULocalPlayer>(Player))
	{
		//Build menu only after game is initialized
		NexosInGameMenu = MakeShareable(new FNexosInGameMenu());
		NexosInGameMenu->Construct(Cast<ULocalPlayer>(Player));

		FInputModeGameOnly InputMode;
		SetInputMode(InputMode);
	}
}

void ANexosPlayerController::QueryAchievements()
{

}

void ANexosPlayerController::OnQueryAchievementsComplete(const FUniqueNetId& PlayerId, const bool bWasSuccessful)
{
}

void ANexosPlayerController::OnLeaderboardReadComplete(bool bWasSuccessful)
{

}

void ANexosPlayerController::QueryStats()
{

}

void ANexosPlayerController::UnFreeze()
{
	ServerRestartPlayer();
}

void ANexosPlayerController::FailedToSpawnPawn()
{
	if (StateName == NAME_Inactive)
	{
		BeginInactiveState();
	}
	Super::FailedToSpawnPawn();
}

void ANexosPlayerController::PawnPendingDestroy(APawn* P)
{
	LastDeathLocation = P->GetActorLocation();
	FVector CameraLocation = LastDeathLocation + FVector(0, 0, 300.0f);
	FRotator CameraRotation(-90.0f, 0.0f, 0.0f);
	FindDeathCameraSpot(CameraLocation, CameraRotation);

	Super::PawnPendingDestroy(P);

	ClientSetSpectatorCamera(CameraLocation, CameraRotation);
}

void ANexosPlayerController::GameHasEnded(class AActor* EndGameFocus, bool bIsWinner)
{
	Super::GameHasEnded(EndGameFocus, bIsWinner);
}

void ANexosPlayerController::ClientSetSpectatorCamera_Implementation(FVector CameraLocation, FRotator CameraRotation)
{
	SetInitialLocationAndRotation(CameraLocation, CameraRotation);
	SetViewTarget(this);
}

bool ANexosPlayerController::FindDeathCameraSpot(FVector& CameraLocation, FRotator& CameraRotation)
{
	const FVector PawnLocation = GetPawn()->GetActorLocation();
	FRotator ViewDir = GetControlRotation();
	ViewDir.Pitch = -45.0f;

	const float YawOffsets[] = { 0.0f, -180.0f, 90.0f, -90.0f, 45.0f, -45.0f, 135.0f, -135.0f };
	const float CameraOffset = 600.0f;
	FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(DeathCamera), true, GetPawn());

	FHitResult HitResult;
	for (int32 i = 0; i < UE_ARRAY_COUNT(YawOffsets); i++)
	{
		FRotator CameraDir = ViewDir;
		CameraDir.Yaw += YawOffsets[i];
		CameraDir.Normalize();

		const FVector TestLocation = PawnLocation - CameraDir.Vector() * CameraOffset;

		const bool bBlocked = GetWorld()->LineTraceSingleByChannel(HitResult, PawnLocation, TestLocation, ECC_Camera, TraceParams);

		if (!bBlocked)
		{
			CameraLocation = TestLocation;
			CameraRotation = CameraDir;
			return true;
		}
	}

	return false;
}

bool ANexosPlayerController::ServerCheat_Validate(const FString& Msg)
{
	return true;
}

void ANexosPlayerController::ServerCheat_Implementation(const FString& Msg)
{
	if (CheatManager)
	{
		ClientMessage(ConsoleCommand(Msg));
	}
}

void ANexosPlayerController::SimulateInputKey(FKey Key, bool bPressed)
{
	InputKey(Key, bPressed ? IE_Pressed : IE_Released, 1, false);
}

void ANexosPlayerController::OnKill()
{

}


void ANexosPlayerController::OnDeathMessage(class ANexosPlayerState* KillerPlayerState, class ANexosPlayerState* KilledPlayerState, const UDamageType* KillerDamageType)
{

}

void ANexosPlayerController::UpdateAchievementProgress(const FString& Id, float Percent)
{

}

void ANexosPlayerController::OnToggleInGameMenu()
{
	if (GEngine->GameViewport == nullptr)
	{
		return;
	}

	// this is not ideal, but necessary to prevent both players from pausing at the same time on the same frame
	UWorld* GameWorld = GEngine->GameViewport->GetWorld();

	for (auto It = GameWorld->GetControllerIterator(); It; ++It)
	{
		ANexosPlayerController* Controller = Cast<ANexosPlayerController>(*It);
		if (Controller && Controller->IsPaused())
		{
			return;
		}
	}

	// if no one's paused, pause
	if (NexosInGameMenu.IsValid())
	{
		NexosInGameMenu->ToggleGameMenu();
	}
}

void ANexosPlayerController::OnConditionalCloseScoreboard()
{
	ANexosHUD* NexosHUD = GetNexosHUD();
	if (NexosHUD && (NexosHUD->IsMatchOver() == false))
	{
		NexosHUD->ConditionalCloseScoreboard();
	}
}

void ANexosPlayerController::OnToggleScoreboard()
{
	ANexosHUD* NexosHUD = GetNexosHUD();
	if (NexosHUD && (NexosHUD->IsMatchOver() == false))
	{
		NexosHUD->ToggleScoreboard();
	}
}

void ANexosPlayerController::OnShowScoreboard()
{
	ANexosHUD* NexosHUD = GetNexosHUD();
	if (NexosHUD)
	{
		NexosHUD->ShowScoreboard(true);
	}
}

void ANexosPlayerController::OnHideScoreboard()
{
	ANexosHUD* NexosHUD = GetNexosHUD();
	// If have a valid match and the match is over - hide the scoreboard
	if ((NexosHUD != NULL) && (NexosHUD->IsMatchOver() == false))
	{
		NexosHUD->ShowScoreboard(false);
	}
}

bool ANexosPlayerController::IsGameMenuVisible() const
{
	bool Result = false;
	if (NexosInGameMenu.IsValid())
	{
		Result = NexosInGameMenu->GetIsGameMenuUp();
	}

	return Result;
}

void ANexosPlayerController::SetInfiniteAmmo(bool bEnable)
{
	bInfiniteAmmo = bEnable;
}

void ANexosPlayerController::SetInfiniteClip(bool bEnable)
{
	bInfiniteClip = bEnable;
}

void ANexosPlayerController::SetHealthRegen(bool bEnable)
{
	bHealthRegen = bEnable;
}

void ANexosPlayerController::SetGodMode(bool bEnable)
{
	bGodMode = bEnable;
}

void ANexosPlayerController::SetIsVibrationEnabled(bool bEnable)
{
	bIsVibrationEnabled = bEnable;
}

void ANexosPlayerController::ClientGameStarted_Implementation()
{

}

/** Starts the online game using the session name in the PlayerState */
void ANexosPlayerController::ClientStartOnlineGame_Implementation()
{

}

/** Ends the online game using the session name in the PlayerState */
void ANexosPlayerController::ClientEndOnlineGame_Implementation()
{

}

void ANexosPlayerController::HandleReturnToMainMenu()
{
	OnHideScoreboard();
	CleanupSessionOnReturnToMenu();
}

void ANexosPlayerController::ClientReturnToMainMenu_Implementation(const FString& InReturnReason)
{
	const UWorld* World = GetWorld();
	UNexosGameInstance* SGI = World != NULL ? Cast<UNexosGameInstance>(World->GetGameInstance()) : NULL;

	if (!ensure(SGI != NULL))
	{
		return;
	}

	if (GetNetMode() == NM_Client)
	{
		const FText ReturnReason = NSLOCTEXT("NetworkErrors", "HostQuit", "The host has quit the match.");
		const FText OKButton = NSLOCTEXT("DialogButtons", "OKAY", "OK");

		//SGI->ShowMessageThenGotoState(ReturnReason, OKButton, FText::GetEmpty(), NexosGameInstanceState::TitleMenu);
	}
	else
	{
		//SGI->GotoState(NexosGameInstanceState::TitleMenu);
	}

	// Clear the flag so we don't do normal end of round stuff next
	bGameEndedFrame = false;
}

/** Ends and/or destroys game session */
void ANexosPlayerController::CleanupSessionOnReturnToMenu()
{
	const UWorld* World = GetWorld();
	UNexosGameInstance* SGI = World != NULL ? Cast<UNexosGameInstance>(World->GetGameInstance()) : NULL;

	if (ensure(SGI != NULL))
	{
		SGI->CleanupSessionOnReturnToMenu();
	}
}

void ANexosPlayerController::ClientGameEnded_Implementation(class AActor* EndGameFocus, bool bIsWinner)
{
	Super::ClientGameEnded_Implementation(EndGameFocus, bIsWinner);

	// Disable controls now the game has ended
	SetIgnoreMoveInput(true);

	bAllowGameActions = false;

	// Make sure that we still have valid view target
	SetViewTarget(GetPawn());

	ANexosHUD* NexosHUD = GetNexosHUD();
	if (NexosHUD)
	{
		NexosHUD->SetMatchState(bIsWinner ? ENexosMatchState::Won : ENexosMatchState::Lost);
	}

	UpdateSaveFileOnGameEnd(bIsWinner);
	UpdateAchievementsOnGameEnd();
	UpdateLeaderboardsOnGameEnd();
	UpdateStatsOnGameEnd(bIsWinner);

	// Flag that the game has just ended (if it's ended due to host loss we want to wait for ClientReturnToMainMenu_Implementation first, incase we don't want to process)
	bGameEndedFrame = true;
}

void ANexosPlayerController::ClientSendRoundEndEvent_Implementation(bool bIsWinner, int32 ExpendedTimeInSeconds)
{

}

void ANexosPlayerController::SetCinematicMode(bool bInCinematicMode, bool bHidePlayer, bool bAffectsHUD, bool bAffectsMovement, bool bAffectsTurning)
{
	Super::SetCinematicMode(bInCinematicMode, bHidePlayer, bAffectsHUD, bAffectsMovement, bAffectsTurning);

	// If we have a pawn we need to determine if we should show/hide the weapon
	ANexosPlayerCharacter* MyPawn = Cast<ANexosPlayerCharacter>(GetPawn());
	ANexosWeaponBase* MyWeapon = MyPawn ? MyPawn->GetWeapon() : NULL;
	if (MyWeapon)
	{
		if (bInCinematicMode && bHidePlayer)
		{
			MyWeapon->SetActorHiddenInGame(true);
		}
		else if (!bCinematicMode)
		{
			MyWeapon->SetActorHiddenInGame(false);
		}
	}
}

bool ANexosPlayerController::IsMoveInputIgnored() const
{
	if (IsInState(NAME_Spectating))
	{
		return false;
	}
	else
	{
		return Super::IsMoveInputIgnored();
	}
}

bool ANexosPlayerController::IsLookInputIgnored() const
{
	if (IsInState(NAME_Spectating))
	{
		return false;
	}
	else
	{
		return Super::IsLookInputIgnored();
	}
}

void ANexosPlayerController::InitInputSystem()
{
	Super::InitInputSystem();

	UNexosPersistentUser* PersistentUser = GetPersistentUser();
	if (PersistentUser)
	{
		PersistentUser->TellInputAboutKeybindings();
	}
}

void ANexosPlayerController::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ANexosPlayerController, bInfiniteAmmo, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ANexosPlayerController, bInfiniteClip, COND_OwnerOnly);

	DOREPLIFETIME(ANexosPlayerController, bHealthRegen);
}

void ANexosPlayerController::Suicide()
{
	if (IsInState(NAME_Playing))
	{
		ServerSuicide();
	}
}

bool ANexosPlayerController::ServerSuicide_Validate()
{
	return true;
}

void ANexosPlayerController::ServerSuicide_Implementation()
{
	if ((GetPawn() != NULL) && ((GetWorld()->TimeSeconds - GetPawn()->CreationTime > 1) || (GetNetMode() == NM_Standalone)))
	{
		ANexosPlayerCharacter* MyPawn = Cast<ANexosPlayerCharacter>(GetPawn());
		if (MyPawn)
		{
			MyPawn->Suicide();
		}
	}
}

bool ANexosPlayerController::HasInfiniteAmmo() const
{
	return bInfiniteAmmo;
}

bool ANexosPlayerController::HasInfiniteClip() const
{
	return bInfiniteClip;
}

bool ANexosPlayerController::HasHealthRegen() const
{
	return bHealthRegen;
}

bool ANexosPlayerController::HasGodMode() const
{
	return bGodMode;
}

bool ANexosPlayerController::IsVibrationEnabled() const
{
	return bIsVibrationEnabled;
}

bool ANexosPlayerController::IsGameInputAllowed() const
{
	return bAllowGameActions && !bCinematicMode;
}

void ANexosPlayerController::ToggleChatWindow()
{
	ANexosHUD* NexosHUD = Cast<ANexosHUD>(GetHUD());
	if (NexosHUD)
	{
		NexosHUD->ToggleChat();
	}
}

void ANexosPlayerController::ClientTeamMessage_Implementation(APlayerState* SenderPlayerState, const FString& S, FName Type, float MsgLifeTime)
{
	ANexosHUD* NexosHUD = Cast<ANexosHUD>(GetHUD());
	if (NexosHUD)
	{
		if (Type == ServerSayString)
		{
			if (SenderPlayerState != PlayerState)
			{
				NexosHUD->AddChatLine(FText::FromString(S), false);
			}
		}
	}
}

void ANexosPlayerController::Say(const FString& Msg)
{
	ServerSay(Msg.Left(128));
}

bool ANexosPlayerController::ServerSay_Validate(const FString& Msg)
{
	return true;
}

void ANexosPlayerController::ServerSay_Implementation(const FString& Msg)
{
	GetWorld()->GetAuthGameMode<ANexosColosseumGameModeBase>()->Broadcast(this, Msg, ServerSayString);
}

ANexosHUD* ANexosPlayerController::GetNexosHUD() const
{
	return Cast<ANexosHUD>(GetHUD());
}


UNexosPersistentUser* ANexosPlayerController::GetPersistentUser() const
{
	UNexosLocalPlayer* const NexosLocalPlayer = Cast<UNexosLocalPlayer>(Player);
	return NexosLocalPlayer ? NexosLocalPlayer->GetPersistentUser() : nullptr;
}

bool ANexosPlayerController::SetPause(bool bPause, FCanUnpause CanUnpauseDelegate /*= FCanUnpause()*/)
{
	return true;
}

FVector ANexosPlayerController::GetFocalLocation() const
{
	const ANexosPlayerCharacter* NexosCharacter = Cast<ANexosPlayerCharacter>(GetPawn());

	// On death we want to use the player's death cam location rather than the location of where the pawn is at the moment
	// This guarantees that the clients see their death cam correctly, as their pawns have delayed destruction.
	if (NexosCharacter && NexosCharacter->bIsDying)
	{
		return GetSpawnLocation();
	}

	return Super::GetFocalLocation();
}

void ANexosPlayerController::ShowInGameMenu()
{
	ANexosHUD* NexosHUD = GetNexosHUD();
	if (NexosInGameMenu.IsValid() && !NexosInGameMenu->GetIsGameMenuUp() && NexosHUD && (NexosHUD->IsMatchOver() == false))
	{
		NexosInGameMenu->ToggleGameMenu();
	}
}
void ANexosPlayerController::UpdateAchievementsOnGameEnd()
{

}

void ANexosPlayerController::UpdateLeaderboardsOnGameEnd()
{


}

void ANexosPlayerController::UpdateStatsOnGameEnd(bool bIsWinner)
{

}


void ANexosPlayerController::UpdateSaveFileOnGameEnd(bool bIsWinner)
{
	ANexosPlayerState* NexosPlayerState = Cast<ANexosPlayerState>(PlayerState);
	if (NexosPlayerState)
	{
		// update local saved profile
		UNexosPersistentUser* const PersistentUser = GetPersistentUser();
		if (PersistentUser)
		{
			PersistentUser->AddMatchResults(NexosPlayerState->GetKills(), NexosPlayerState->GetDeaths(), NexosPlayerState->GetNumBulletsFired(), bIsWinner);
			PersistentUser->SaveIfDirty();
		}
	}
}

void ANexosPlayerController::PreClientTravel(const FString& PendingURL, ETravelType TravelType, bool bIsSeamlessTravel)
{

	Super::PreClientTravel(PendingURL, TravelType, bIsSeamlessTravel);

	if (const UWorld* World = GetWorld())
	{
		UNexosViewportClient* NexosViewport = Cast<UNexosViewportClient>(World->GetGameViewport());

		if (NexosViewport != NULL)
		{
			//NexosViewport->ShowLoadingScreen();
		}

		ANexosHUD* NexosHUD = Cast<ANexosHUD>(GetHUD());
		if (NexosHUD != nullptr)
		{
			// Passing true to bFocus here ensures that focus is returned to the game viewport.
			NexosHUD->ShowScoreboard(false, true);
		}
	}
}

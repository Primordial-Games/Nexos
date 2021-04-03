// Fill out your copyright notice in the Description page of Project Settings.


#include "Online/NexosColosseumGameModeBase.h"
#include "Nexos.h"
#include "NexosGameInstance.h"
#include "UI/NexosHUD.h"
#include "Player/NexosSpectatorPawn.h"
#include "Player/NexosPlayerCharacter.h"
#include "Online/NexosColosseumGameState.h"
#include "Online/NexosPlayerState.h"
#include "Online/NexosGameSession.h"
#include "Bots/ShooterAIController.h"
#include "NexosTeamStart.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "EngineUtils.h"

ANexosColosseumGameModeBase::ANexosColosseumGameModeBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<APawn> BotPawnOb(TEXT("/Game/Blueprints/Pawns/Bots_and_AI/Colosseum/NexosBot"));
	BotPawnClass = BotPawnOb.Class;

	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnOb(TEXT("/Game/Blueprints/Pawns/NexosPlayerPawn"));
	DefaultPawnClass = PlayerPawnOb.Class;

	static ConstructorHelpers::FClassFinder<AHUD> PlayerHUD(TEXT("/Game/UI/HUD/PlayerHUD"));
	HUDClass = PlayerHUD.Class;
	PlayerControllerClass = ANexosPlayerController::StaticClass();
	PlayerStateClass = ANexosPlayerState::StaticClass();
	SpectatorClass = ANexosSpectatorPawn::StaticClass();
	GameStateClass = ANexosColosseumGameState::StaticClass();
	ReplaySpectatorPlayerControllerClass = ANexosPlayerController::StaticClass();

	MinRespawnDelay = 3.0f;

	bAllowBots = true;
	bNeedsBotCreation = true;
	bUseSeamlessTravel = true;
}

void ANexosColosseumGameModeBase::PostInitProperties()
{
	Super::PostInitProperties();
	if (PlatformPlayerControllerClass != nullptr)
	{
		PlayerControllerClass = PlatformPlayerControllerClass;
	}
}

FString ANexosColosseumGameModeBase::GetBotsCountOptionName()
{
	return FString(TEXT("Bots"));
}

void ANexosColosseumGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	const int32 BotsCountOptionValue = UGameplayStatics::GetIntOption(Options, GetBotsCountOptionName(), 0);
	SetAllowBots(BotsCountOptionValue > 0 ? true : false, BotsCountOptionValue);
	Super::InitGame(MapName, Options, ErrorMessage);

	const UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance && Cast<UNexosGameInstance>(GameInstance)->GetOnlineMode() != EOnlineMode::Offline)
	{
		bPauseable = false;
	}
}

void ANexosColosseumGameModeBase::SetAllowBots(bool bInAllowBots, int32 InMaxBots)
{
	bAllowBots = bInAllowBots;
	MaxBots = InMaxBots;
}

/** return what session calss to use */
TSubclassOf<AGameSession> ANexosColosseumGameModeBase::GetGameSessionClass() const
{
	return ANexosGameSession::StaticClass();
}

void ANexosColosseumGameModeBase::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	GetWorldTimerManager().SetTimer(TimerHandle_DefaultTimer, this, &ANexosColosseumGameModeBase::DefaultTimer, GetWorldSettings()->GetEffectiveTimeDilation(), true);
}

void ANexosColosseumGameModeBase::DefaultTimer()
{
	// don't update timers for Play In Editor mode, not a real match
	if (GetWorld()->IsPlayInEditor())
	{
		if (GetMatchState() == MatchState::WaitingToStart)
		{
			StartMatch();
		}
		return;
	}

	ANexosColosseumGameState* const MyGameState = Cast<ANexosColosseumGameState>(GameState);
	if (MyGameState && MyGameState->RemainingTime > 0 && !MyGameState->bTimerPaused)
	{
		MyGameState->RemainingTime--;

		if (MyGameState->RemainingTime <= 0)
		{
			if (GetMatchState() == MatchState::WaitingPostMatch)
			{
				RequestFinishAndExitToMainMenu();
			}
			else if (GetMatchState() == MatchState::InProgress)
			{
				FinishMatch();

				// send end round event
				for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
				{
					ANexosPlayerController* PlayerController = Cast<ANexosPlayerController>(*It);

					if (PlayerController && MyGameState)
					{
						ANexosPlayerState* PlayerState = Cast<ANexosPlayerState>((*It)->PlayerState);
						const bool bIsWinner = IsWinner(PlayerState);

						PlayerController->ClientSendRoundEndEvent(bIsWinner, MyGameState->ElapsedTime);
					}
				}
			}
			else if (GetMatchState() == MatchState::WaitingToStart)
			{
				StartMatch();
			}
		}
	}
}

void ANexosColosseumGameModeBase::HandleMatchIsWaitingToStart()
{
	Super::HandleMatchIsWaitingToStart();

	if (bNeedsBotCreation)
	{
		CreateBotControllers();
		bNeedsBotCreation = false;
	}

	if (bDelayedStart)
	{
		// start warmup if needed
		ANexosColosseumGameState* const MyGameState = Cast<ANexosColosseumGameState>(GameState);
		if (MyGameState && MyGameState->RemainingTime == 0)
		{
			MyGameState->RemainingTime = WarmupTime;
		}
		else
		{
			MyGameState->RemainingTime = 0.0f;
		}
	}
}

void ANexosColosseumGameModeBase::HandleMatchHasStarted()
{
	bNeedsBotCreation = true;
	Super::HandleMatchHasStarted();

	ANexosColosseumGameState* const MyGameState = Cast<ANexosColosseumGameState>(GameState);
	MyGameState->RemainingTime = RoundTime;
	StartBots();

	// notify players
	for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
	{
		ANexosPlayerController* PC = Cast<ANexosPlayerController>(*It);
		if (PC)
		{
			PC->ClientGameStarted();
		}
	}
}

void ANexosColosseumGameModeBase::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);
}

void ANexosColosseumGameModeBase::FinishMatch()
{
	ANexosColosseumGameState* const MyGameState = Cast<ANexosColosseumGameState>(GameState);
	if (IsMatchInProgress())
	{
		EndMatch();
		DetermineMatchWinner();

		// notify players
		for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
		{
			ANexosPlayerState* PlayerState = Cast<ANexosPlayerState>((*It)->PlayerState);
			const bool bIsWinner = IsWinner(PlayerState);

			(*It)->GameHasEnded(NULL, bIsWinner);
		}

		// lock all pawns
		// pawns are not marked as keep for seamless travel, so we will create new pawns on the next match rather than
		// turning these back on.
		for (APawn* Pawn : TActorRange<APawn>(GetWorld()))
		{
			Pawn->TurnOff();
		}

		// set up to restart the match
		MyGameState->RemainingTime = TimeBetweenGames;
		GetWorld()->ServerTravel("/Game/Maps/Menus/MultiplayerLobby?game=MPLobby");
	}
}

void ANexosColosseumGameModeBase::RequestFinishAndExitToMainMenu()
{
	FinishMatch();
	GetWorld()->ServerTravel("/Game/Maps/Menus/MultiplayerLobby?game=MPLobby");
}

void ANexosColosseumGameModeBase::DetermineMatchWinner()
{
	// Need to do nothing here
}

bool ANexosColosseumGameModeBase::IsWinner(class ANexosPlayerState* PlayerState) const
{
	return false;
}

void ANexosColosseumGameModeBase::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	ANexosColosseumGameState* const MyGameState = Cast<ANexosColosseumGameState>(GameState);
	const bool bMatchIsOver = MyGameState && MyGameState->HasMatchEnded();
	if (bMatchIsOver)
	{
		ErrorMessage = TEXT("Match is over!");
	}
	else
	{
		// GameSession can be NULL if the match is over
		Super::PreLogin(Options, Address, FUniqueNetIdRepl(), ErrorMessage);
	}
}

void ANexosColosseumGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	// update spectator location for client
	ANexosPlayerController* NewPC = Cast<ANexosPlayerController>(NewPlayer);
	if (NewPC && NewPC->GetPawn() == NULL)
	{
		NewPC->ClientSetSpectatorCamera(NewPC->GetSpawnLocation(), NewPC->GetControlRotation());
	}

	// notify new player if match is already in progress
	if (NewPC && IsMatchInProgress())
	{
		NewPC->ClientGameStarted();
		NewPC->ClientStartOnlineGame();
	}
}

void ANexosColosseumGameModeBase::Killed(AController* Killer, AController* KilledPlayer, APawn* KilledPawn, const UDamageType* DamageType)
{
	ANexosPlayerState* KillerPlayerState = Killer ? Cast<ANexosPlayerState>(Killer->PlayerState) : NULL;
	ANexosPlayerState* VictimPlayerState = KilledPlayer ? Cast<ANexosPlayerState>(KilledPlayer->PlayerState) : NULL;

	if (KillerPlayerState && KillerPlayerState != VictimPlayerState)
	{
		KillerPlayerState->ScoreKill(VictimPlayerState, KillScore);
		KillerPlayerState->InformAboutKill(KillerPlayerState, DamageType, VictimPlayerState);
	}

	if (VictimPlayerState)
	{
		VictimPlayerState->ScoreDeath(KillerPlayerState, DeathScore);
		VictimPlayerState->BroadcastDeath(KillerPlayerState, DamageType, VictimPlayerState);
	}
}

float ANexosColosseumGameModeBase::ModifyDamage(float Damage, AActor* DamagedActor, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) const
{
	float ActualDamage = Damage;

	ANexosPlayerCharacter* DamagedPawn = Cast<ANexosPlayerCharacter>(DamagedActor);
	if (DamagedPawn && EventInstigator)
	{
		ANexosPlayerState* DamagedPlayerState = Cast<ANexosPlayerState>(DamagedPawn->GetPlayerState());
		ANexosPlayerState* InstigatorPlayerState = Cast<ANexosPlayerState>(EventInstigator->PlayerState);

		// disable friendly fire
		if (!CanDealDamage(InstigatorPlayerState, DamagedPlayerState))
		{
			ActualDamage = 0.0f;
		}

		// scale self damage
		if (InstigatorPlayerState == DamagedPlayerState)
		{
			ActualDamage *= DamageSelfScale;
		}
	}

	return ActualDamage;
}

bool ANexosColosseumGameModeBase::CanDealDamage(class ANexosPlayerState* DamageInstigator, class ANexosPlayerState* DamagedPlayer) const
{
	return true;
}

bool ANexosColosseumGameModeBase::AllowCheats(APlayerController* P)
{
	return false;
}

bool ANexosColosseumGameModeBase::ShouldSpawnAtStartSpot(AController* Player)
{
	return false;
}

UClass* ANexosColosseumGameModeBase::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	if (InController->IsA<AShooterAIController>())
	{
		return BotPawnClass;
	}

	return Super::GetDefaultPawnClassForController_Implementation(InController);
}

void ANexosColosseumGameModeBase::RestartPlayer(AController* NewPlayer)
{
	Super::RestartPlayer(NewPlayer);

	ANexosPlayerController* PC = Cast<ANexosPlayerController>(NewPlayer);
	if (PC)
	{
		// Since initial weapon is equipped before the pawn is added to the replication graph, need to resend the notify so that it can be added as a dependent actor
		ANexosPlayerCharacter* Character = Cast<ANexosPlayerCharacter>(PC->GetCharacter());
		if (Character)
		{
			ANexosPlayerCharacter::NotifyEquipWeapon.Broadcast(Character, Character->GetWeapon());
		}
		PC->ClientGameStarted();
	}
}

AActor* ANexosColosseumGameModeBase::ChoosePlayerStart_Implementation(AController* Player)
{
	TArray<APlayerStart*> PreferredSpawns;
	TArray<APlayerStart*> FallbackSpawns;

	APlayerStart* BestStart = NULL;
	for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
	{
		APlayerStart* TestSpawn = *It;
		if (TestSpawn->IsA<APlayerStartPIE>())
		{
			// Always prefer the first "Play from Here" PlayerStart, if we find one while in PIE mode
			BestStart = TestSpawn;
			break;
		}
		else
		{
			if (IsSpawnpointAllowed(TestSpawn, Player))
			{
				if (IsSpawnpointPreferred(TestSpawn, Player))
				{
					PreferredSpawns.Add(TestSpawn);
				}
				else
				{
					FallbackSpawns.Add(TestSpawn);
				}
			}
		}
	}

	if (BestStart == NULL)
	{
		if (PreferredSpawns.Num() > 0)
		{
			BestStart = PreferredSpawns[FMath::RandHelper(PreferredSpawns.Num())];
		}
		else if (FallbackSpawns.Num() > 0)
		{
			BestStart = FallbackSpawns[FMath::RandHelper(FallbackSpawns.Num())];
		}
	}

	return BestStart ? BestStart : Super::ChoosePlayerStart_Implementation(Player);
}

bool ANexosColosseumGameModeBase::IsSpawnpointAllowed(APlayerStart* SpawnPoint, AController* Player) const
{
	ANexosTeamStart* ShooterSpawnPoint = Cast<ANexosTeamStart>(SpawnPoint);
	if (ShooterSpawnPoint)
	{
		AShooterAIController* AIController = Cast<AShooterAIController>(Player);
		if (ShooterSpawnPoint->bNotForBots && AIController)
		{
			return false;
		}

		if (ShooterSpawnPoint->bNotForPlayers && AIController == NULL)
		{
			return false;
		}
		return true;
	}

	return false;
}

bool ANexosColosseumGameModeBase::IsSpawnpointPreferred(APlayerStart* SpawnPoint, AController* Player) const
{
	ACharacter* MyPawn = Cast<ACharacter>((*DefaultPawnClass)->GetDefaultObject<ACharacter>());
	AShooterAIController* AIController = Cast<AShooterAIController>(Player);
	if (AIController != nullptr)
	{
		MyPawn = Cast<ACharacter>(BotPawnClass->GetDefaultObject<ACharacter>());
	}

	if (MyPawn)
	{
		const FVector SpawnLocation = SpawnPoint->GetActorLocation();
		for (ACharacter* OtherPawn : TActorRange<ACharacter>(GetWorld()))
		{
			if (OtherPawn != MyPawn)
			{
				const float CombinedHeight = (MyPawn->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + OtherPawn->GetCapsuleComponent()->GetScaledCapsuleHalfHeight()) * 2.0f;
				const float CombinedRadius = MyPawn->GetCapsuleComponent()->GetScaledCapsuleRadius() + OtherPawn->GetCapsuleComponent()->GetScaledCapsuleRadius();
				const FVector OtherLocation = OtherPawn->GetActorLocation();

				// check if player start overlaps this pawn
				if (FMath::Abs(SpawnLocation.Z - OtherLocation.Z) < CombinedHeight && (SpawnLocation - OtherLocation).Size2D() < CombinedRadius)
				{
					return false;
				}
			}
		}
	}
	else
	{
		return false;
	}

	return true;
}

void ANexosColosseumGameModeBase::CreateBotControllers()
{
	UWorld* World = GetWorld();
	int32 ExistingBots = 0;
	for (FConstControllerIterator It = World->GetControllerIterator(); It; ++It)
	{
		AShooterAIController* AIC = Cast<AShooterAIController>(*It);
		if (AIC)
		{
			++ExistingBots;
		}
	}

	// Create any necessary AIControllers.  Hold off on Pawn creation until pawns are actually necessary or need recreating.	
	int32 BotNum = ExistingBots;
	for (int32 i = 0; i < MaxBots - ExistingBots; ++i)
	{
		CreateBot(BotNum + i);
	}
}

AShooterAIController* ANexosColosseumGameModeBase::CreateBot(int32 BotNum)
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Instigator = nullptr;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnInfo.OverrideLevel = nullptr;

	UWorld* World = GetWorld();
	AShooterAIController* AIC = World->SpawnActor<AShooterAIController>(SpawnInfo);
	InitBot(AIC, BotNum);

	return AIC;
}

void ANexosColosseumGameModeBase::StartBots()
{
	// checking number of existing human player.
	UWorld* World = GetWorld();
	for (FConstControllerIterator It = World->GetControllerIterator(); It; ++It)
	{
		AShooterAIController* AIC = Cast<AShooterAIController>(*It);
		if (AIC)
		{
			RestartPlayer(AIC);
		}
	}
}

void ANexosColosseumGameModeBase::InitBot(AShooterAIController* AIController, int32 BotNum)
{
	if (AIController)
	{
		if (AIController->PlayerState)
		{
			FString BotName = FString::Printf(TEXT("Bot %d"), BotNum);
			AIController->PlayerState->SetPlayerName(BotName);
		}
	}
}

void ANexosColosseumGameModeBase::RestartGame()
{
	// Hide the scoreboard too !
	for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
	{
		ANexosPlayerController* PlayerController = Cast<ANexosPlayerController>(*It);
		if (PlayerController != nullptr)
		{
			ANexosHUD* NexosHUD = Cast<ANexosHUD>(PlayerController->GetHUD());
			if (NexosHUD != nullptr)
			{
				// Passing true to bFocus here ensures that focus is returned to the game viewport.
				NexosHUD->ShowScoreboard(false, true);
			}
		}
	}

	Super::RestartGame();
}
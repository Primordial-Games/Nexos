// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/NexosPlayerCharacter.h"
#include "Player/NexosPlayerCharacterMovement.h"
#include "Player/NexosPlayerController.h"
#include "Nexos.h"
#include "Weapons/NexosWeaponBase.h"
#include "Weapons/NexosDamageType.h"
#include "UI/NexosHUD.h"
#include "Online/NexosPlayerState.h"
#include "Online/NexosColosseumGameModeBase.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimInstance.h"
#include "AudioThread.h"
#include "Sound/SoundCue.h"
#include "Components/CapsuleComponent.h"


static int32 NetVisualizeRelevancyTestPoints = 0;
FAutoConsoleVariableRef CVarNetVisualizeRelevancyTestPoints(
	TEXT("p.NetVisualizeRelevancyTestPoints"),
	NetVisualizeRelevancyTestPoints,
	TEXT("")
	TEXT("0: Disable, 1: Enable"),
	ECVF_Cheat);


static int32 NetEnablePauseRelevancy = 1;
FAutoConsoleVariableRef CVarNetEnablePauseRelevancy(
	TEXT("p.NetEnablePauseRelevancy"),
	NetEnablePauseRelevancy,
	TEXT("")
	TEXT("0: Disable, 1: Enable"),
	ECVF_Cheat);

FOnNexosPlayerCharacterEquipWeapon ANexosPlayerCharacter::NotifyEquipWeapon;
FOnNexosPlayerCharacterUnEquipWeapon ANexosPlayerCharacter::NotifyUnEquipWeapon;

ANexosPlayerCharacter::ANexosPlayerCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UNexosPlayerCharacterMovement>(ACharacter::CharacterMovementComponentName))
{
	Mesh1P = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("PawnMesh1P"));
	Mesh1P->SetupAttachment(GetCapsuleComponent());
	Mesh1P->bOnlyOwnerSee = true;
	Mesh1P->bOwnerNoSee = false;
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->bReceivesDecals = false;
	Mesh1P->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
	Mesh1P->PrimaryComponentTick.TickGroup = TG_PrePhysics;
	Mesh1P->SetCollisionObjectType(ECC_Pawn);
	Mesh1P->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh1P->SetCollisionResponseToAllChannels(ECR_Ignore);


	GetMesh()->bOnlyOwnerSee = false;
	GetMesh()->bOwnerNoSee = true;
	GetMesh()->bReceivesDecals = false;
	GetMesh()->SetCollisionObjectType(ECC_Pawn);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(COLLISION_PROJECTILE, ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_PROJECTILE, ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Ignore);

	MainMat = GetMesh()->GetMaterial(0);

	TargetingSpeedModifier = 0.5f;
	bIsTargeting = false;
	RunningSpeedModifier = 1.5f;
	bWantsToRun = false;
	bWantsToFire = false;
	bIsTakingDamage = false;
	LowHealthPercentage = 0.8f;
	LowSheildsPercentage = 0.7f;

	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	DefaultLoadoutClasses.Add("Primary", NULL );
	DefaultLoadoutClasses.Add("Special", NULL);
	DefaultLoadoutClasses.Add("Power", NULL);
	DefaultLoadoutClasses.Add("Sidearm", NULL);


}

void ANexosPlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	CurrentMaterLevel = 0.0f;


	if (GetLocalRole() == ROLE_Authority)
	{
		Health = GetMaxHealth();
		Shields = GetMaxShields();

		// Needs to happen after character is added to repgraph
		GetWorldTimerManager().SetTimerForNextTick(this, &ANexosPlayerCharacter::SpawnDefaultInventory);
	}

	// set initial mesh visibility (3rd person view)
	UpdatePawnMeshes();

	// create material instance for setting team colors (3rd person view)
	for (int32 iMat = 0; iMat < GetMesh()->GetNumMaterials(); iMat++)
	{
		MeshMIDs.Add(GetMesh()->CreateAndSetMaterialInstanceDynamic(iMat));
	}

	// play respawn effects
	if (GetNetMode() != NM_DedicatedServer)
	{
		if (RespawnFX)
		{
			UGameplayStatics::SpawnEmitterAtLocation(this, RespawnFX, Mesh1P->GetComponentLocation(), GetActorRotation());
		}

		if (RespawnMat0)
		{
			RespawnMatterial0 = UMaterialInstanceDynamic::Create(RespawnMat0, this);
			GetMesh()->SetMaterial(0, RespawnMatterial0);

			RespawnMatterial1 = UMaterialInstanceDynamic::Create(RespawnMat1, this);
			GetMesh()->SetMaterial(1, RespawnMatterial1);

			UMaterialInstanceDynamic* Mesh1PMID = Mesh1P->CreateAndSetMaterialInstanceDynamic(0);

			UpdateTeamColors(Mesh1PMID);

			// set initial mesh visibility (3rd person view)
			UpdatePawnMeshes();

			// create material instance for setting team colors (3rd person view)
			for (int32 iMat = 0; iMat < GetMesh()->GetNumMaterials(); iMat++)
			{
				MeshMIDs.Add(GetMesh()->CreateAndSetMaterialInstanceDynamic(iMat));
			}
			GetWorld()->GetTimerManager().SetTimer(MaterializationTimer, this, &ANexosPlayerCharacter::MaterializeEffect, 0.1f, true);
		}

		if (RespawnSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, RespawnSound, GetActorLocation());
		}

	}
}

void ANexosPlayerCharacter::Destroyed()
{
	Super::Destroyed();
	DestroyInventory();
}

void ANexosPlayerCharacter::PawnClientRestart()
{
	Super::PawnClientRestart();

	// switch mesh to 1st person view
	UpdatePawnMeshes();

	// reattach weapon if needed
	SetCurrentWeapon(CurrentWeapon);

	// set team colors for 1st person view
	UMaterialInstanceDynamic* Mesh1PMID = Mesh1P->CreateAndSetMaterialInstanceDynamic(0);
	UpdateTeamColors(Mesh1PMID);
}

void ANexosPlayerCharacter::PossessedBy(class AController* InController)
{
	Super::PossessedBy(InController);

	// [server] as soon as PlayerState is assigned, set team colors of this pawn for local player
	UpdateTeamColorsAllMIDs();
}

void ANexosPlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// [client] as soon as PlayerState is assigned, set team colors of this pawn for local player
	if (GetPlayerState() != NULL)
	{
		UpdateTeamColorsAllMIDs();
	}
}

FRotator ANexosPlayerCharacter::GetAimOffsets() const
{
	const FVector AimDirWS = GetBaseAimRotation().Vector();
	const FVector AimDirLS = ActorToWorld().InverseTransformVectorNoScale(AimDirWS);
	const FRotator AimRotLS = AimDirLS.Rotation();

	return AimRotLS;
}

bool ANexosPlayerCharacter::IsEnemyFor(AController* TestPC) const
{
	if (TestPC == Controller || TestPC == NULL)
	{
		return false;
	}

	ANexosPlayerState* TestPlayerState = Cast<ANexosPlayerState>(TestPC->PlayerState);
	ANexosPlayerState* MyPlayerState = Cast<ANexosPlayerState>(GetPlayerState());

	bool bIsEnemy = true;
	if (GetWorld()->GetGameState())
	{
		const ANexosColosseumGameModeBase* DefGame = GetWorld()->GetGameState()->GetDefaultGameMode<ANexosColosseumGameModeBase>();
		if (DefGame && MyPlayerState && TestPlayerState)
		{
			bIsEnemy = DefGame->CanDealDamage(TestPlayerState, MyPlayerState);
		}
	}
	return bIsEnemy;
}

//////////////////////////////////////////////////////////////////////////
// Meshes

void ANexosPlayerCharacter::UpdatePawnMeshes()
{
	bool const bFirstPerson = IsFirstPerson();

	Mesh1P->VisibilityBasedAnimTickOption = !bFirstPerson ? EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered : EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
	Mesh1P->SetOwnerNoSee(!bFirstPerson);

	GetMesh()->VisibilityBasedAnimTickOption = bFirstPerson ? EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered : EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
	GetMesh()->SetOwnerNoSee(bFirstPerson);
}

void ANexosPlayerCharacter::UpdateTeamColors(UMaterialInstanceDynamic* UseMID)
{
	if (UseMID)
	{
		ANexosPlayerState* MyPlayerState = Cast<ANexosPlayerState>(GetPlayerState());
		if (MyPlayerState != NULL)
		{
			float MaterialParam = (float)MyPlayerState->GetTeamNum();
			UseMID->SetScalarParameterValue(TEXT("Team Color Index"), MaterialParam);
		}
	}
}

void ANexosPlayerCharacter::MaterializeEffect()
{
	if (CurrentMaterLevel < 1.0f)
	{
		RespawnMatterial0->SetScalarParameterValue(TEXT("Materialize Amount"), CurrentMaterLevel + .03f);
		RespawnMatterial1->SetScalarParameterValue(TEXT("Materialize Amount"), CurrentMaterLevel + .03f);
		CurrentMaterLevel += .03;
	}
	else
	{
		GetMesh()->SetMaterial(0, MainMat);

		UMaterialInstanceDynamic* Mesh1PMID = GetMesh()->CreateAndSetMaterialInstanceDynamic(0);

		UpdateTeamColors(Mesh1PMID);
		// create material instance for setting team colors (3rd person view)
		for (int32 iMat = 0; iMat < GetMesh()->GetNumMaterials(); iMat++)
		{
			MeshMIDs.Add(GetMesh()->CreateAndSetMaterialInstanceDynamic(iMat));

		}

		GetWorld()->GetTimerManager().ClearTimer(MaterializationTimer);
	}
}

void ANexosPlayerCharacter::OnCameraUpdate(const FVector& CameraLocation, const FRotator& CameraRotation)
{
	/*
	USkeletalMeshComponent* DefMesh1P = Cast<USkeletalMeshComponent>(GetClass()->GetDefaultSubobjectByName(TEXT("PawnMesh1P")));
	const FMatrix DefMeshLS = FRotationTranslationMatrix(DefMesh1P->GetRelativeRotation(), DefMesh1P->GetRelativeLocation());
	const FMatrix LocalToWorld = ActorToWorld().ToMatrixWithScale();

	// Mesh rotating code expect uniform scale in LocalToWorld matrix

	const FRotator RotCameraPitch(CameraRotation.Pitch, 0.0f, 0.0f);
	const FRotator RotCameraYaw(0.0f, CameraRotation.Yaw, 0.0f);

	const FMatrix LeveledCameraLS = FRotationTranslationMatrix(RotCameraYaw, CameraLocation) * LocalToWorld.Inverse();
	const FMatrix PitchedCameraLS = FRotationMatrix(RotCameraPitch) * LeveledCameraLS;
	const FMatrix MeshRelativeToCamera = DefMeshLS * LeveledCameraLS.Inverse();
	const FMatrix PitchedMesh = MeshRelativeToCamera * PitchedCameraLS;

	Mesh1P->SetRelativeLocationAndRotation(PitchedMesh.GetOrigin(), PitchedMesh.Rotator());
	*/
}

//////////////////////////////////////////////////////////////////////////
// Damage & death

void ANexosPlayerCharacter::FellOutOfWorld(const class UDamageType& dmgType)
{
	Die(Health, FDamageEvent(dmgType.GetClass()), NULL, NULL);
}

void ANexosPlayerCharacter::Suicide()
{
	KilledBy(this);
}

void ANexosPlayerCharacter::KilledBy(APawn* EventInstigator)
{
	if (GetLocalRole() == ROLE_Authority && !bIsDying)
	{
		AController* Killer = NULL;
		if (EventInstigator != NULL)
		{
			Killer = EventInstigator->Controller;
			LastHitBy = NULL;
		}

		Die(Health, FDamageEvent(UDamageType::StaticClass()), Killer, NULL);
	}
}

float ANexosPlayerCharacter::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	bIsTakingDamage = true;
	GetWorld()->GetTimerManager().ClearTimer(HealthRegenDelayHandle);
	GetWorld()->GetTimerManager().ClearTimer(ShieldRegenDelayHandle);

	ANexosPlayerController* MyPC = Cast<ANexosPlayerController>(Controller);
	if (MyPC && MyPC->HasGodMode())
	{
		return 0.f;
	}
	if (Health <= 0.f)
	{
		return 0.f;
	}

	// Need to re-work 
	ANexosColosseumGameModeBase* const Game = GetWorld()->GetAuthGameMode<ANexosColosseumGameModeBase>();
	Damage = Game ? Game->ModifyDamage(Damage, this, DamageEvent, EventInstigator, DamageCauser) : 0.f;

	const float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	if (ActualDamage > 0.f)
	{
		if (Shields > 0.f)
		{
			if (Shields < ActualDamage)
			{
				float CurrentShields = Shields;
				float RolloverDamage = ActualDamage - CurrentShields;
				Shields = 0.f;
				//todo add sheild breaK effect
				Health -= RolloverDamage;

			}
			else
			{
				Shields -= ActualDamage;
				//todo add sheild hit material
			}
		}
		else
		{
			Health -= ActualDamage;
		}

		if (Health <= 0)
		{
			Die(ActualDamage, DamageEvent, EventInstigator, DamageCauser);
		}
		else
		{
			PlayHit(ActualDamage, DamageEvent, EventInstigator ? EventInstigator->GetPawn() : NULL, DamageCauser);
		}

		MakeNoise(1.0f, EventInstigator ? EventInstigator->GetPawn() : this);
	}
	GetWorld()->GetTimerManager().SetTimer(HealthRegenDelayHandle, this, &ANexosPlayerCharacter::HealthRegenWait, HealthRegenDelay, false);
	GetWorld()->GetTimerManager().SetTimer(ShieldRegenDelayHandle, this, &ANexosPlayerCharacter::ShieldRegenWait, ShieldRegenDelay, false);
	return ActualDamage;
}

void ANexosPlayerCharacter::HealthRegenWait()
{
	bIsTakingDamage = false;
	GetWorld()->GetTimerManager().SetTimer(HealthRegenRateHandle, this, &ANexosPlayerCharacter::RegenerateHealth, 0.01f, true);
}

void ANexosPlayerCharacter::RegenerateHealth()
{
	if (!bIsTakingDamage && Health < GetMaxHealth())
	{
		Health += HealthRegenRate / 100;
	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(HealthRegenRateHandle);
	}
}

void ANexosPlayerCharacter::ShieldRegenWait()
{
	bIsTakingDamage = false;
	GetWorld()->GetTimerManager().SetTimer(ShieldRegenRateHandle, this, &ANexosPlayerCharacter::RegenerateShield, 0.01f, true);
}

void ANexosPlayerCharacter::RegenerateShield()
{
	if (!bIsTakingDamage && Shields < GetMaxShields())
	{
		Shields += ShieldRegenRate / 100;
	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(ShieldRegenRateHandle);
	}
}




bool ANexosPlayerCharacter::CanDie(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser) const
{
	if (bIsDying										// already dying
		|| IsPendingKill()								// already destroyed
		|| GetLocalRole() != ROLE_Authority				// not authority
		|| GetWorld()->GetAuthGameMode<ANexosColosseumGameModeBase>() == NULL
		|| GetWorld()->GetAuthGameMode<ANexosColosseumGameModeBase>()->GetMatchState() == MatchState::LeavingMap)	// level transition occurring
	{
		return false;
	}

	return true;
}

bool ANexosPlayerCharacter::Die(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser)
{
	if (!CanDie(KillingDamage, DamageEvent, Killer, DamageCauser))
	{
		return false;
	}

	Health = FMath::Min(0.0f, Health);

	// if this is an environmental death then refer to the previous killer so that they receive credit (knocked into lava pits, etc)
	UDamageType const* const DamageType = DamageEvent.DamageTypeClass ? DamageEvent.DamageTypeClass->GetDefaultObject<UDamageType>() : GetDefault<UDamageType>();
	Killer = GetDamageInstigator(Killer, *DamageType);

	AController* const KilledPlayer = (Controller != NULL) ? Controller : Cast<AController>(GetOwner());
	GetWorld()->GetAuthGameMode<ANexosColosseumGameModeBase>()->Killed(Killer, KilledPlayer, this, DamageType);

	NetUpdateFrequency = GetDefault<ANexosPlayerCharacter>()->NetUpdateFrequency;
	GetCharacterMovement()->ForceReplicationUpdate();

	OnDeath(KillingDamage, DamageEvent, Killer ? Killer->GetPawn() : NULL, DamageCauser);

	return true;
}

void ANexosPlayerCharacter::OnDeath(float KillingDamage, struct FDamageEvent const& DamageEvent, class APawn* PawnInstigator, class AActor* DamageCauser)
{
	if (bIsDying)
	{
		return;
	}

	SetReplicatingMovement(false);
	TearOff();
	bIsDying = true;

	if (GetLocalRole() == ROLE_Authority)
	{
		ReplicateHit(KillingDamage, DamageEvent, PawnInstigator, DamageCauser, true);

		// play the force feedback effect on the client player controller
		ANexosPlayerController* PC = Cast<ANexosPlayerController>(Controller);
		if (PC && DamageEvent.DamageTypeClass)
		{
			UNexosDamageType* DamageType = Cast<UNexosDamageType>(DamageEvent.DamageTypeClass->GetDefaultObject());
			if (DamageType && DamageType->KilledForceFeedback && PC->IsVibrationEnabled())
			{
				FForceFeedbackParameters FFPrams;
				FFPrams.Tag = "Damage";
				PC->ClientPlayForceFeedback(DamageType->KilledForceFeedback, FFPrams);
			}

		}
	}

	if (GetNetMode() != NM_DedicatedServer && DeathSound && Mesh1P && Mesh1P->IsVisible())
	{
		UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation());
	}

	// remove all weapons
	DestroyInventory();



	// switch back to 3rd person view
	UpdatePawnMeshes();

	DetachFromControllerPendingDestroy();
	StopAllAnimMontages();

	if (LowHealthWarningPlayer && LowHealthWarningPlayer->IsPlaying())
	{
		LowHealthWarningPlayer->Stop();
	}

	if (LowShieldsWarningPlayer && LowShieldsWarningPlayer->IsPlaying())
	{
		LowShieldsWarningPlayer->Stop();
	}

	if (RunLoopAC)
	{
		RunLoopAC->Stop();
	}

	if (GetMesh())
	{
		static FName CollisionProfileName(TEXT("Ragdoll"));
		GetMesh()->SetCollisionProfileName(CollisionProfileName);
	}
	SetActorEnableCollision(true);

	// Death anim
	float DeathAnimDuration = PlayAnimMontage(DeathAnim);

	// Ragdoll
	if (DeathAnimDuration > 0.f)
	{
		// Trigger ragdoll a little before the animation early so the character doesn't
		// blend back to its normal position.
		const float TriggerRagdollTime = DeathAnimDuration - 0.7f;

		// Enable blend physics so the bones are properly blending against the montage.
		GetMesh()->bBlendPhysics = true;

		// Use a local timer handle as we don't need to store it for later but we don't need to look for something to clear
		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle, this, &ANexosPlayerCharacter::SetRagdollPhysics, FMath::Max(0.1f, TriggerRagdollTime), false);
	}
	else
	{
		SetRagdollPhysics();
	}

	// disable collisions on capsule
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
}

void ANexosPlayerCharacter::PlayHit(float DamageTaken, struct FDamageEvent const& DamageEvent, class APawn* PawnInstigator, class AActor* DamageCauser)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		ReplicateHit(DamageTaken, DamageEvent, PawnInstigator, DamageCauser, false);

		// play the force feedback effect on the client player controller
		ANexosPlayerController* PC = Cast<ANexosPlayerController>(Controller);
		if (PC && DamageEvent.DamageTypeClass)
		{
			UNexosDamageType* DamageType = Cast<UNexosDamageType>(DamageEvent.DamageTypeClass->GetDefaultObject());
			if (DamageType && DamageType->HitForceFeedback && PC->IsVibrationEnabled())
			{
				FForceFeedbackParameters FFParams;
				FFParams.Tag = "Damage";
				PC->ClientPlayForceFeedback(DamageType->HitForceFeedback, FFParams);
			}
		}
	}

	if (DamageTaken > 0.f)
	{
		ApplyDamageMomentum(DamageTaken, DamageEvent, PawnInstigator, DamageCauser);
	}

	ANexosPlayerController* MyPC = Cast<ANexosPlayerController>(Controller);
	ANexosHUD* MyHUD = MyPC ? Cast<ANexosHUD>(MyPC->GetHUD()) : NULL;
	if (MyHUD)
	{
		MyHUD->NotifyWeaponHit(DamageTaken, DamageEvent, PawnInstigator);
	}

	if (PawnInstigator && PawnInstigator != this && PawnInstigator->IsLocallyControlled())
	{
		ANexosPlayerController* InstigatorPC = Cast<ANexosPlayerController>(PawnInstigator->Controller);
		ANexosHUD* InstigatorHUD = InstigatorPC ? Cast<ANexosHUD>(InstigatorPC->GetHUD()) : NULL;
		if (InstigatorHUD)
		{
			InstigatorHUD->NotifyEnemyHit();
		}
	}
}

void ANexosPlayerCharacter::SetRagdollPhysics()
{
	bool bInRagdoll = false;

	if (IsPendingKill())
	{
		bInRagdoll = false;
	}
	else if (!GetMesh() || !GetMesh()->GetPhysicsAsset())
	{
		bInRagdoll = false;
	}
	else
	{
		// initialize physics/etc
		GetMesh()->SetSimulatePhysics(true);
		GetMesh()->WakeAllRigidBodies();
		GetMesh()->bBlendPhysics = true;

		bInRagdoll = true;
	}

	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->SetComponentTickEnabled(false);

	if (!bInRagdoll)
	{
		// hide and set short lifespan
		TurnOff();
		SetActorHiddenInGame(true);
		SetLifeSpan(1.0f);
	}
	else
	{
		SetLifeSpan(10.0f);
	}
}

void ANexosPlayerCharacter::ReplicateHit(float Damage, struct FDamageEvent const& DamageEvent, class APawn* PawnInstigator, class AActor* DamageCauser, bool bKilled)
{
	const float TimeoutTime = GetWorld()->GetTimeSeconds() + 0.5f;

	FDamageEvent const& LastDamageEvent = LastTakeHitInfo.GetDamageEvent();
	if ((PawnInstigator == LastTakeHitInfo.PawnInstigator.Get()) && (LastDamageEvent.DamageTypeClass == LastTakeHitInfo.DamageTypeClass) && (LastTakeHitTimeTimeout == TimeoutTime))
	{
		// same frame damage
		if (bKilled && LastTakeHitInfo.bKilled)
		{
			// Redundant death take hit, just ignore it
			return;
		}

		// otherwise, accumulate damage done this frame
		Damage += LastTakeHitInfo.ActualDamage;
	}

	LastTakeHitInfo.ActualDamage = Damage;
	LastTakeHitInfo.PawnInstigator = Cast<ANexosPlayerCharacter>(PawnInstigator);
	LastTakeHitInfo.DamageCauser = DamageCauser;
	LastTakeHitInfo.SetDamageEvent(DamageEvent);
	LastTakeHitInfo.bKilled = bKilled;
	LastTakeHitInfo.EnsureReplication();

	LastTakeHitTimeTimeout = TimeoutTime;
}

void ANexosPlayerCharacter::OnRep_LastTakeHitInfo()
{
	if (LastTakeHitInfo.bKilled)
	{
		OnDeath(LastTakeHitInfo.ActualDamage, LastTakeHitInfo.GetDamageEvent(), LastTakeHitInfo.PawnInstigator.Get(), LastTakeHitInfo.DamageCauser.Get());
	}
	else
	{
		PlayHit(LastTakeHitInfo.ActualDamage, LastTakeHitInfo.GetDamageEvent(), LastTakeHitInfo.PawnInstigator.Get(), LastTakeHitInfo.DamageCauser.Get());
	}
}

//Pawn::PlayDying sets this lifespan, but when that function is called on client, dead pawn's role is still SimulatedProxy despite bTearOff being true. 
void ANexosPlayerCharacter::TornOff()
{
	SetLifeSpan(25.f);
}

bool ANexosPlayerCharacter::IsMoving()
{
	return FMath::Abs(GetLastMovementInputVector().Size()) > 0.f;
}

//////////////////////////////////////////////////////////////////////////
// Inventory

void ANexosPlayerCharacter::SpawnDefaultInventory()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		return;
	}

	
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	if(!DefaultLoadoutClasses["Primary"] == NULL)
	{
		ANexosWeaponBase* PrimaryWeapon = GetWorld()->SpawnActor<ANexosWeaponBase>(DefaultLoadoutClasses["Primary"], SpawnInfo);
		AddWeapon(PrimaryWeapon);
	}
	if (!DefaultLoadoutClasses["Special"] == NULL)
	{
		ANexosWeaponBase* SpecialWeapon = GetWorld()->SpawnActor<ANexosWeaponBase>(DefaultLoadoutClasses["Special"], SpawnInfo);
		AddWeapon(SpecialWeapon);
	}
	if (!DefaultLoadoutClasses["Power"] == NULL)
	{
		ANexosWeaponBase* PowerWeapon = GetWorld()->SpawnActor<ANexosWeaponBase>(DefaultLoadoutClasses["Power"], SpawnInfo);
		AddWeapon(PowerWeapon);
	}
	if (!DefaultLoadoutClasses["Sidearm"] == NULL)
	{
		ANexosWeaponBase* SidearmWeapon = GetWorld()->SpawnActor<ANexosWeaponBase>(DefaultLoadoutClasses["Sidearm"], SpawnInfo);
		AddWeapon(SidearmWeapon);
	}
	// equip first weapon in inventory
	if (Inventory.Num() > 0)
	{
		EquipWeapon(Inventory[0]);
	}
}

void ANexosPlayerCharacter::DestroyInventory()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		return;
	}

	// remove all weapons from inventory and destroy them
	for (int32 i = Inventory.Num() - 1; i >= 0; i--)
	{
		ANexosWeaponBase* Weapon = Inventory[i];
		if (Weapon)
		{
			RemoveWeapon(Weapon);
			Weapon->Destroy();
		}
	}
}

void ANexosPlayerCharacter::AddWeapon(ANexosWeaponBase* Weapon)
{
	if (Weapon && GetLocalRole() == ROLE_Authority)
	{
		Weapon->OnEnterInventory(this);
		Inventory.AddUnique(Weapon);
	}
}

void ANexosPlayerCharacter::RemoveWeapon(ANexosWeaponBase* Weapon)
{
	if (Weapon && GetLocalRole() == ROLE_Authority)
	{
		Weapon->OnLeaveInventory();
		Inventory.RemoveSingle(Weapon);
	}
}

ANexosWeaponBase* ANexosPlayerCharacter::FindWeapon(TSubclassOf<ANexosWeaponBase> WeaponClass)
{
	for (int32 i = 0; i < Inventory.Num(); i++)
	{
		if (Inventory[i] && Inventory[i]->IsA(WeaponClass))
		{
			return Inventory[i];
		}
	}

	return NULL;
}

void ANexosPlayerCharacter::EquipWeapon(ANexosWeaponBase* Weapon)
{
	if (Weapon)
	{
		if (GetLocalRole() == ROLE_Authority)
		{
			SetCurrentWeapon(Weapon, CurrentWeapon);
		}
		else
		{
			ServerEquipWeapon(Weapon);
		}
	}
}

bool ANexosPlayerCharacter::ServerEquipWeapon_Validate(ANexosWeaponBase* Weapon)
{
	return true;
}

void ANexosPlayerCharacter::ServerEquipWeapon_Implementation(ANexosWeaponBase* Weapon)
{
	EquipWeapon(Weapon);
}

void ANexosPlayerCharacter::OnRep_CurrentWeapon(ANexosWeaponBase* LastWeapon)
{
	SetCurrentWeapon(CurrentWeapon, LastWeapon);
}

void ANexosPlayerCharacter::SetCurrentWeapon(ANexosWeaponBase* NewWeapon, ANexosWeaponBase* LastWeapon)
{
	ANexosWeaponBase* LocalLastWeapon = nullptr;

	if (LastWeapon != NULL)
	{
		LocalLastWeapon = LastWeapon;
	}
	else if (NewWeapon != CurrentWeapon)
	{
		LocalLastWeapon = CurrentWeapon;
	}

	// unequip previous
	if (LocalLastWeapon)
	{
		LocalLastWeapon->OnUnEquip();
	}

	CurrentWeapon = NewWeapon;

	// equip new one
	if (NewWeapon)
	{
		NewWeapon->SetOwningPawn(this);	// Make sure weapon's MyPawn is pointing back to us. During replication, we can't guarantee APawn::CurrentWeapon will rep after AWeapon::MyPawn!

		NewWeapon->OnEquip(LastWeapon);

	}
}


//////////////////////////////////////////////////////////////////////////
// Weapon usage

void ANexosPlayerCharacter::StartWeaponFire()
{
	if (!bWantsToFire)
	{
		bWantsToFire = true;
		if (CurrentWeapon)
		{
			CurrentWeapon->StartFire();
		}
	}
}

void ANexosPlayerCharacter::StopWeaponFire()
{
	if (bWantsToFire)
	{
		bWantsToFire = false;
		if (CurrentWeapon)
		{
			CurrentWeapon->StopFire();
		}
	}
}

bool ANexosPlayerCharacter::CanFire() const
{
	return IsAlive();
}

bool ANexosPlayerCharacter::CanReload() const
{
	return true;
}

void ANexosPlayerCharacter::SetTargeting(bool bNewTargeting)
{
	bIsTargeting = bNewTargeting;

	if (TargetingSound)
	{
		UGameplayStatics::SpawnSoundAttached(TargetingSound, GetRootComponent());
	}

	if (GetLocalRole() < ROLE_Authority)
	{
		ServerSetTargeting(bNewTargeting);
	}
}

bool ANexosPlayerCharacter::ServerSetTargeting_Validate(bool bNewTargeting)
{
	return true;
}

void ANexosPlayerCharacter::ServerSetTargeting_Implementation(bool bNewTargeting)
{
	SetTargeting(bNewTargeting);
}

//////////////////////////////////////////////////////////////////////////
// Movement

void ANexosPlayerCharacter::SetRunning(bool bNewRunning, bool bToggle)
{
	bWantsToRun = bNewRunning;
	bWantsToRunToggled = bNewRunning && bToggle;

	if (GetLocalRole() < ROLE_Authority)
	{
		ServerSetRunning(bNewRunning, bToggle);
	}
}

bool ANexosPlayerCharacter::ServerSetRunning_Validate(bool bNewRunning, bool bToggle)
{
	return true;
}

void ANexosPlayerCharacter::ServerSetRunning_Implementation(bool bNewRunning, bool bToggle)
{
	SetRunning(bNewRunning, bToggle);
}

void ANexosPlayerCharacter::UpdateRunSounds()
{
	const bool bIsRunSoundPlaying = RunLoopAC != nullptr && RunLoopAC->IsActive();
	const bool bWantsRunSoundPlaying = IsRunning() && IsMoving();

	// Don't bother playing the sounds unless we're running and moving.
	if (!bIsRunSoundPlaying && bWantsRunSoundPlaying)
	{
		if (RunLoopAC != nullptr)
		{
			RunLoopAC->Play();
		}
		else if (RunLoopSound != nullptr)
		{
			RunLoopAC = UGameplayStatics::SpawnSoundAttached(RunLoopSound, GetRootComponent());
			if (RunLoopAC != nullptr)
			{
				RunLoopAC->bAutoDestroy = false;
			}
		}
	}
	else if (bIsRunSoundPlaying && !bWantsRunSoundPlaying)
	{
		RunLoopAC->Stop();
		if (RunStopSound != nullptr)
		{
			UGameplayStatics::SpawnSoundAttached(RunStopSound, GetRootComponent());
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Animations

float ANexosPlayerCharacter::PlayAnimMontage(class UAnimMontage* AnimMontage, float InPlayRate, FName StartSectionName)
{
	USkeletalMeshComponent* UseMesh = GetPawnMesh();
	if (AnimMontage && UseMesh && UseMesh->AnimScriptInstance)
	{
		return UseMesh->AnimScriptInstance->Montage_Play(AnimMontage, InPlayRate);
	}

	return 0.0f;
}

void ANexosPlayerCharacter::StopAnimMontage(class UAnimMontage* AnimMontage)
{
	USkeletalMeshComponent* UseMesh = GetPawnMesh();
	if (AnimMontage && UseMesh && UseMesh->AnimScriptInstance &&
		UseMesh->AnimScriptInstance->Montage_IsPlaying(AnimMontage))
	{
		UseMesh->AnimScriptInstance->Montage_Stop(AnimMontage->BlendOut.GetBlendTime(), AnimMontage);
	}
}

void ANexosPlayerCharacter::StopAllAnimMontages()
{
	USkeletalMeshComponent* UseMesh = GetPawnMesh();
	if (UseMesh && UseMesh->AnimScriptInstance)
	{
		UseMesh->AnimScriptInstance->Montage_Stop(0.0f);
	}
}


//////////////////////////////////////////////////////////////////////////
// Input

void ANexosPlayerCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveForward", this, &ANexosPlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ANexosPlayerCharacter::MoveRight);
	PlayerInputComponent->BindAxis("MoveUp", this, &ANexosPlayerCharacter::MoveUp);
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ANexosPlayerCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ANexosPlayerCharacter::LookUpAtRate);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ANexosPlayerCharacter::OnStartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ANexosPlayerCharacter::OnStopFire);

	PlayerInputComponent->BindAction("ADS", IE_Pressed, this, &ANexosPlayerCharacter::OnStartTargeting);
	PlayerInputComponent->BindAction("ADS", IE_Released, this, &ANexosPlayerCharacter::OnStopTargeting);

	PlayerInputComponent->BindAction("NextWeapon", IE_Pressed, this, &ANexosPlayerCharacter::OnNextWeapon);
	PlayerInputComponent->BindAction("PrevWeapon", IE_Pressed, this, &ANexosPlayerCharacter::OnPrevWeapon);

	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &ANexosPlayerCharacter::OnReload);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ANexosPlayerCharacter::OnStartJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ANexosPlayerCharacter::OnStopJump);

	PlayerInputComponent->BindAction("Run", IE_Pressed, this, &ANexosPlayerCharacter::OnStartRunning);
	PlayerInputComponent->BindAction("RunToggle", IE_Pressed, this, &ANexosPlayerCharacter::OnStartRunningToggle);
	PlayerInputComponent->BindAction("Run", IE_Released, this, &ANexosPlayerCharacter::OnStopRunning);

}


void ANexosPlayerCharacter::MoveForward(float Val)
{
	if (Controller && Val != 0.f)
	{
		// Limit pitch when walking or falling
		const bool bLimitRotation = (GetCharacterMovement()->IsMovingOnGround() || GetCharacterMovement()->IsFalling());
		const FRotator Rotation = bLimitRotation ? GetActorRotation() : Controller->GetControlRotation();
		const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::X);
		AddMovementInput(Direction, Val);
	}
}

void ANexosPlayerCharacter::MoveRight(float Val)
{
	if (Val != 0.f)
	{
		const FQuat Rotation = GetActorQuat();
		const FVector Direction = FQuatRotationMatrix(Rotation).GetScaledAxis(EAxis::Y);
		AddMovementInput(Direction, Val);
	}
}

void ANexosPlayerCharacter::MoveUp(float Val)
{
	if (Val != 0.f)
	{
		// Not when walking or falling.
		if (GetCharacterMovement()->IsMovingOnGround() || GetCharacterMovement()->IsFalling())
		{
			return;
		}

		AddMovementInput(FVector::UpVector, Val);
	}
}

void ANexosPlayerCharacter::TurnAtRate(float Val)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Val * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ANexosPlayerCharacter::LookUpAtRate(float Val)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Val * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}


void ANexosPlayerCharacter::OnStartFire()
{
	ANexosPlayerController* MyPC = Cast<ANexosPlayerController>(Controller);
	if (MyPC && MyPC->IsGameInputAllowed())
	{
		if (IsRunning())
		{
			SetRunning(false, false);
		}
		StartWeaponFire();
	}

}

void ANexosPlayerCharacter::OnStopFire()
{
	StopWeaponFire();
}

void ANexosPlayerCharacter::OnStartTargeting()
{
	ANexosPlayerController* MyPC = Cast<ANexosPlayerController>(Controller);
	if (MyPC && MyPC->IsGameInputAllowed())
	{
		if (IsRunning())
		{
			SetRunning(false, false);
		}
		SetTargeting(true);

		SetCamera(true);
	}
}

void ANexosPlayerCharacter::OnStopTargeting()
{
	SetTargeting(false);
	SetCamera(false);
}

void ANexosPlayerCharacter::SetCamera_Implementation(bool bIsTargetingActive)
{

}

void ANexosPlayerCharacter::OnNextWeapon()
{
	ANexosPlayerController* MyPC = Cast<ANexosPlayerController>(Controller);
	if (MyPC && MyPC->IsGameInputAllowed())
	{
		if (Inventory.Num() >= 2 && (CurrentWeapon == NULL || CurrentWeapon->GetCurrentState() != EWeaponState::Equipping))
		{
			const int32 CurrentWeaponIdx = Inventory.IndexOfByKey(CurrentWeapon);
			ANexosWeaponBase* NextWeapon = Inventory[(CurrentWeaponIdx - 1 + Inventory.Num()) % Inventory.Num()];
			EquipWeapon(NextWeapon);
		}
	}
}

void ANexosPlayerCharacter::OnPrevWeapon()
{
	ANexosPlayerController* MyPC = Cast<ANexosPlayerController>(Controller);
	if (MyPC && MyPC->IsGameInputAllowed())
	{
		if (Inventory.Num() >= 2 && (CurrentWeapon == NULL || CurrentWeapon->GetCurrentState() != EWeaponState::Equipping))
		{
			const int32 CurrentWeaponIdx = Inventory.IndexOfByKey(CurrentWeapon);
			ANexosWeaponBase* PrevWeapon = Inventory[(CurrentWeaponIdx + 1) % Inventory.Num()];
			EquipWeapon(PrevWeapon);
		}
	}
}

void ANexosPlayerCharacter::OnReload()
{
	ANexosPlayerController* MyPC = Cast<ANexosPlayerController>(Controller);
	if (MyPC && MyPC->IsGameInputAllowed())
	{
		if (CurrentWeapon)
		{
			CurrentWeapon->StartReload();
		}
	}
}

void ANexosPlayerCharacter::OnStartRunning()
{
	ANexosPlayerController* MyPC = Cast<ANexosPlayerController>(Controller);
	if (MyPC && MyPC->IsGameInputAllowed())
	{
		if (IsTargeting())
		{
			SetTargeting(false);
		}
		StopWeaponFire();
		SetRunning(true, false);
	}
}

void ANexosPlayerCharacter::OnStartRunningToggle()
{
	ANexosPlayerController* MyPC = Cast<ANexosPlayerController>(Controller);
	if (MyPC && MyPC->IsGameInputAllowed())
	{
		if (IsTargeting())
		{
			SetTargeting(false);
		}
		StopWeaponFire();
		SetRunning(true, true);
	}
}

void ANexosPlayerCharacter::OnStopRunning()
{
	SetRunning(false, false);
}

bool ANexosPlayerCharacter::IsRunning() const
{
	if (!GetCharacterMovement())
	{
		return false;
	}

	return (bWantsToRun || bWantsToRunToggled) && !GetVelocity().IsZero() && (GetVelocity().GetSafeNormal2D() | GetActorForwardVector()) > -0.1;
}

void ANexosPlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bWantsToRunToggled && !IsRunning())
	{
		SetRunning(false, false);
	}
	ANexosPlayerController* MyPC = Cast<ANexosPlayerController>(Controller);
	if (MyPC && MyPC->HasHealthRegen())
	{
		if (this->Health < this->GetMaxHealth())
		{
			this->Health += 5 * DeltaSeconds;
			if (Health > this->GetMaxHealth())
			{
				Health = this->GetMaxHealth();
			}
		}
	}

	if (GEngine->UseSound())
	{
		if (LowHealthSound)
		{
			if ((this->Health > 0 && this->Health < this->GetMaxHealth() * LowHealthPercentage) && (!LowHealthWarningPlayer || !LowHealthWarningPlayer->IsPlaying()))
			{
				LowHealthWarningPlayer = UGameplayStatics::SpawnSoundAttached(LowHealthSound, GetRootComponent(),
					NAME_None, FVector(ForceInit), EAttachLocation::KeepRelativeOffset, true);
				if (LowHealthWarningPlayer)
				{
					LowHealthWarningPlayer->SetVolumeMultiplier(0.0f);
				}
			}
			else if ((this->Health > this->GetMaxHealth() * LowHealthPercentage || this->Health < 0) && LowHealthWarningPlayer && LowHealthWarningPlayer->IsPlaying())
			{
				LowHealthWarningPlayer->Stop();
			}
			if (LowHealthWarningPlayer && LowHealthWarningPlayer->IsPlaying())
			{
				const float MinVolume = 0.3f;
				const float VolumeMultiplier = (1.0f - (this->Health / (this->GetMaxHealth() * LowHealthPercentage)));
				LowHealthWarningPlayer->SetVolumeMultiplier(MinVolume + (1.0f - MinVolume) * VolumeMultiplier);
			}
		}

		if (LowSheildsSound)
		{
			if ((this->Shields > 0 && this->Shields < this->GetMaxShields() * LowSheildsPercentage) && (!LowShieldsWarningPlayer || !LowShieldsWarningPlayer->IsPlaying()))
			{
				LowShieldsWarningPlayer = UGameplayStatics::SpawnSoundAttached(LowSheildsSound, GetRootComponent(),
					NAME_None, FVector(ForceInit), EAttachLocation::KeepRelativeOffset, true);
				if (LowShieldsWarningPlayer)
				{
					LowShieldsWarningPlayer->SetVolumeMultiplier(0.0f);
				}
			}
			else if ((this->Shields > this->GetMaxShields() * LowSheildsPercentage || this->Shields < 0) && LowShieldsWarningPlayer && LowShieldsWarningPlayer->IsPlaying())
			{
				LowShieldsWarningPlayer->Stop();
			}
			if (LowShieldsWarningPlayer && LowShieldsWarningPlayer->IsPlaying())
			{
				const float MinVolume = 0.3f;
				const float VolumeMultiplier = (1.0f - (this->Shields / (this->GetMaxShields() * LowSheildsPercentage)));
				LowShieldsWarningPlayer->SetVolumeMultiplier(MinVolume + (1.0f - MinVolume) * VolumeMultiplier);
			}
		}

		UpdateRunSounds();
	}

	const APlayerController* PC = Cast<APlayerController>(GetController());
	const bool bLocallyControlled = (PC ? PC->IsLocalController() : false);
	const uint32 UniqueID = GetUniqueID();
	FAudioThread::RunCommandOnAudioThread([UniqueID, bLocallyControlled]()
		{
		});

	TArray<FVector> PointsToTest;
	BuildPauseReplicationCheckPoints(PointsToTest);

	if (NetVisualizeRelevancyTestPoints == 1)
	{
		for (FVector PointToTest : PointsToTest)
		{
			DrawDebugSphere(GetWorld(), PointToTest, 10.0f, 8, FColor::Red);
		}
	}
}

void ANexosPlayerCharacter::BeginDestroy()
{
	Super::BeginDestroy();

	if (!GExitPurge)
	{
		const uint32 UniqueID = GetUniqueID();
		FAudioThread::RunCommandOnAudioThread([UniqueID]()
			{

			});
	}
}

void ANexosPlayerCharacter::OnStartJump()
{
	ANexosPlayerController* MyPC = Cast<ANexosPlayerController>(Controller);
	if (MyPC && MyPC->IsGameInputAllowed())
	{
		bPressedJump = true;
	}
}

void ANexosPlayerCharacter::OnStopJump()
{
	bPressedJump = false;
	StopJumping();
}

//////////////////////////////////////////////////////////////////////////
// Replication

void ANexosPlayerCharacter::PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker)
{
	Super::PreReplication(ChangedPropertyTracker);

	// Only replicate this property for a short duration after it changes so join in progress players don't get spammed with fx when joining late
	DOREPLIFETIME_ACTIVE_OVERRIDE(ANexosPlayerCharacter, LastTakeHitInfo, GetWorld() && GetWorld()->GetTimeSeconds() < LastTakeHitTimeTimeout);
}

void ANexosPlayerCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// only to local owner: weapon change requests are locally instigated, other clients don't need it
	DOREPLIFETIME_CONDITION(ANexosPlayerCharacter, Inventory, COND_OwnerOnly);

	// everyone except local owner: flag change is locally instigated
	DOREPLIFETIME_CONDITION(ANexosPlayerCharacter, bIsTargeting, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(ANexosPlayerCharacter, bWantsToRun, COND_SkipOwner);

	DOREPLIFETIME_CONDITION(ANexosPlayerCharacter, LastTakeHitInfo, COND_Custom);

	// everyone
	DOREPLIFETIME(ANexosPlayerCharacter, CurrentWeapon);
	DOREPLIFETIME(ANexosPlayerCharacter, bIsTakingDamage);
	DOREPLIFETIME(ANexosPlayerCharacter, MaterializationTimer);

	DOREPLIFETIME(ANexosPlayerCharacter, Health);
	DOREPLIFETIME(ANexosPlayerCharacter, HealthRegenDelay);
	DOREPLIFETIME(ANexosPlayerCharacter, HealthRegenDelayHandle);
	DOREPLIFETIME(ANexosPlayerCharacter, HealthRegenRate);
	DOREPLIFETIME(ANexosPlayerCharacter, HealthRegenRateHandle);

	DOREPLIFETIME(ANexosPlayerCharacter, Shields);
	DOREPLIFETIME(ANexosPlayerCharacter, ShieldRegenDelay);
	DOREPLIFETIME(ANexosPlayerCharacter, ShieldRegenDelayHandle);
	DOREPLIFETIME(ANexosPlayerCharacter, ShieldRegenRate);
	DOREPLIFETIME(ANexosPlayerCharacter, ShieldRegenRateHandle);
}

bool ANexosPlayerCharacter::IsReplicationPausedForConnection(const FNetViewer& ConnectionOwnerNetViewer)
{
	if (NetEnablePauseRelevancy == 1)
	{
		APlayerController* PC = Cast<APlayerController>(ConnectionOwnerNetViewer.InViewer);
		check(PC);

		FVector ViewLocation;
		FRotator ViewRotation;
		PC->GetPlayerViewPoint(ViewLocation, ViewRotation);

		FCollisionQueryParams CollisionParams(SCENE_QUERY_STAT(LineOfSight), true, PC->GetPawn());
		CollisionParams.AddIgnoredActor(this);

		TArray<FVector> PointsToTest;
		BuildPauseReplicationCheckPoints(PointsToTest);

		for (FVector PointToTest : PointsToTest)
		{
			if (!GetWorld()->LineTraceTestByChannel(PointToTest, ViewLocation, ECC_Visibility, CollisionParams))
			{
				return false;
			}
		}

		return true;
	}

	return false;
}

void ANexosPlayerCharacter::OnReplicationPausedChanged(bool bIsReplicationPaused)
{
	GetMesh()->SetHiddenInGame(bIsReplicationPaused, true);
}

ANexosWeaponBase* ANexosPlayerCharacter::GetWeapon() const
{
	return CurrentWeapon;
}

int32 ANexosPlayerCharacter::GetInventoryCount() const
{
	return Inventory.Num();
}

ANexosWeaponBase* ANexosPlayerCharacter::GetInventoryWeapon(int32 index) const
{
	return Inventory[index];
}

USkeletalMeshComponent* ANexosPlayerCharacter::GetPawnMesh() const
{
	return IsFirstPerson() ? Mesh1P : GetMesh();
}

USkeletalMeshComponent* ANexosPlayerCharacter::GetSpecifcPawnMesh(bool WantFirstPerson) const
{
	return WantFirstPerson == true ? Mesh1P : GetMesh();
}

FName ANexosPlayerCharacter::GetWeaponAttachPoint() const
{
	return WeaponAttachPoint;
}

float ANexosPlayerCharacter::GetTargetingSpeedModifier() const
{
	return TargetingSpeedModifier;
}

bool ANexosPlayerCharacter::IsTargeting() const
{
	return bIsTargeting;
}

float ANexosPlayerCharacter::GetRunningSpeedModifier() const
{
	return RunningSpeedModifier;
}

bool ANexosPlayerCharacter::IsFiring() const
{
	return bWantsToFire;
};

bool ANexosPlayerCharacter::IsFirstPerson() const
{
	return IsAlive() && Controller && Controller->IsLocalPlayerController();
}

int32 ANexosPlayerCharacter::GetMaxShields() const
{
	return GetClass()->GetDefaultObject<ANexosPlayerCharacter>()->Shields;
}

int32 ANexosPlayerCharacter::GetMaxHealth() const
{
	return GetClass()->GetDefaultObject<ANexosPlayerCharacter>()->Health;
}

bool ANexosPlayerCharacter::IsAlive() const
{
	return Health > 0;
}

float ANexosPlayerCharacter::GetLowSheildsPercentage() const
{
	return LowSheildsPercentage;
}

float ANexosPlayerCharacter::GetLowHealthPercentage() const
{
	return LowHealthPercentage;
}

void ANexosPlayerCharacter::UpdateTeamColorsAllMIDs()
{
	for (int32 i = 0; i < MeshMIDs.Num(); ++i)
	{
		UpdateTeamColors(MeshMIDs[i]);
	}
}

void ANexosPlayerCharacter::BuildPauseReplicationCheckPoints(TArray<FVector>& RelevancyCheckPoints)
{
	FBoxSphereBounds Bounds = GetCapsuleComponent()->CalcBounds(GetCapsuleComponent()->GetComponentTransform());
	FBox BoundingBox = Bounds.GetBox();
	float XDiff = Bounds.BoxExtent.X * 2;
	float YDiff = Bounds.BoxExtent.Y * 2;

	RelevancyCheckPoints.Add(BoundingBox.Min);
	RelevancyCheckPoints.Add(FVector(BoundingBox.Min.X + XDiff, BoundingBox.Min.Y, BoundingBox.Min.Z));
	RelevancyCheckPoints.Add(FVector(BoundingBox.Min.X, BoundingBox.Min.Y + YDiff, BoundingBox.Min.Z));
	RelevancyCheckPoints.Add(FVector(BoundingBox.Min.X + XDiff, BoundingBox.Min.Y + YDiff, BoundingBox.Min.Z));
	RelevancyCheckPoints.Add(FVector(BoundingBox.Max.X - XDiff, BoundingBox.Max.Y, BoundingBox.Max.Z));
	RelevancyCheckPoints.Add(FVector(BoundingBox.Max.X, BoundingBox.Max.Y - YDiff, BoundingBox.Max.Z));
	RelevancyCheckPoints.Add(FVector(BoundingBox.Max.X - XDiff, BoundingBox.Max.Y - YDiff, BoundingBox.Max.Z));
	RelevancyCheckPoints.Add(BoundingBox.Max);
}

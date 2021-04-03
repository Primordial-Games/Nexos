// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapons/NexosWeaponBase.h"
#include "Nexos.h"
#include "Player/NexosPlayerCharacter.h"
#include "Player/NexosPlayerController.h"
#include "Particles/ParticleSystemComponent.h"
#include "Online/NexosPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Engine/SkeletalMesh.h"
#include "TimerManager.h"
#include "UI/NexosHUD.h"
#include <Runtime\Engine\Classes\Kismet\KismetMathLibrary.h>



ANexosWeaponBase::ANexosWeaponBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	Mesh1P = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("WeaponMesh1P"));
	Mesh1P->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
	Mesh1P->bReceivesDecals = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetCollisionObjectType(ECC_WorldDynamic);
	Mesh1P->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh1P->SetCollisionResponseToAllChannels(ECR_Ignore);
	RootComponent = Mesh1P;

	Mesh3P = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("WeaponMesh3P"));
	Mesh3P->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
	Mesh3P->bReceivesDecals = false;
	Mesh3P->CastShadow = true;
	Mesh3P->bCastHiddenShadow = true;
	Mesh3P->SetCollisionObjectType(ECC_WorldDynamic);
	Mesh3P->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh3P->SetCollisionResponseToAllChannels(ECR_Ignore);
	Mesh3P->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Block);
	Mesh3P->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	Mesh3P->SetCollisionResponseToChannel(COLLISION_PROJECTILE, ECR_Block);
	Mesh3P->SetupAttachment(Mesh1P);

	bLoopedMuzzleFX = false;
	bLoopedFireAnim = false;
	bPlayingFireAnim = false;
	bIsEquipped = false;
	bWantsToFire = false;
	bPendingReload = false;
	bPendingEquip = false;
	CurrentState = EWeaponState::Idle;

	CurrentAmmo = 0;
	CurrentAmmoInClip = 0;
	BurstCounter = 0;
	LastFireTime = 0.0f;

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PrePhysics;
	SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
	bReplicates = true;
	bNetUseOwnerRelevancy = true;
}

void ANexosWeaponBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (WeaponConfig.InitialAmmo > 0)
	{
		CurrentAmmoInClip = FMath::Clamp(WeaponConfig.InitialAmmo, 0, WeaponConfig.AmmoPerClip);
		CurrentAmmo = FMath::Clamp(WeaponConfig.InitialAmmo - CurrentAmmoInClip, 0, WeaponConfig.MaxAmmo);
	}

	DetachMeshFromPawn();
}

void ANexosWeaponBase::Destroyed()
{
	Super::Destroyed();
	StopSimulatingWeaponFire();
}


//////////////////////////////////////////////////////////////////////////
// Inventory

void ANexosWeaponBase::OnEquip(const ANexosWeaponBase* LastWeapon)
{
	AttachMeshToPawn();

	bPendingEquip = true;
	DetermineWeaponState();

	// Only play animation if last weapon is vaild
	if (LastWeapon)
	{
		float Duration = PlayWeaponAnimation(EquipAnim);
		if (Duration <= 0.0f)
		{
			// failsafe
			Duration = 0.5f;
		}
		EquipStartedTime = GetWorld()->GetTimeSeconds();
		EquipDuration = Duration;

		GetWorldTimerManager().SetTimer(TimerHandle_OnEquipFinished, this, &ANexosWeaponBase::OnEquipFinished, Duration, false);
	}
	else
	{
		OnEquipFinished();
	}

	if (MyPawn && MyPawn->IsLocallyControlled())
	{
		PlayWeaponSound(EquipSound);
	}

	ANexosPlayerCharacter::NotifyEquipWeapon.Broadcast(MyPawn, this);
}

void ANexosWeaponBase::OnEquipFinished()
{
	AttachMeshToPawn();

	bIsEquipped = true;
	bPendingEquip = false;

	// Determine the state so that the can reload checks will work
	DetermineWeaponState();

	if (MyPawn)
	{
		// try to reload empty clip
		if (MyPawn->IsLocallyControlled() && CurrentAmmoInClip <= 0 && CanReload())
		{
			StartReload();
		}
	}

}

void ANexosWeaponBase::OnUnEquip()
{
	DetachMeshFromPawn();
	bIsEquipped = false;
	StopFire();

	if (bPendingReload)
	{
		StopWeaponAnimation(ReloadAnim);
		bPendingReload = false;

		GetWorldTimerManager().ClearTimer(TimerHandle_StopReload);
		GetWorldTimerManager().ClearTimer(TimerHandle_ReloadWeapon);
	}

	if (bPendingEquip)
	{
		StopWeaponAnimation(EquipAnim);
		bPendingEquip = false;
		GetWorldTimerManager().ClearTimer(TimerHandle_OnEquipFinished);
	}

	ANexosPlayerCharacter::NotifyUnEquipWeapon.Broadcast(MyPawn, this);

	DetermineWeaponState();
}

void ANexosWeaponBase::OnEnterInventory(ANexosPlayerCharacter* NewOwner)
{
	SetOwningPawn(NewOwner);
}

void ANexosWeaponBase::OnLeaveInventory()
{
	if (IsAttachedToPawn())
	{
		OnUnEquip();
	}

	if (GetLocalRole() == ROLE_Authority)
	{
		SetOwningPawn(NULL);
	}
}

void ANexosWeaponBase::AttachMeshToPawn()
{
	if (MyPawn)
	{
		// Remove and hide both 1P and 3P meshes
		DetachMeshFromPawn();

		// For locally controller players we attach both weapons and let the bOnlyOwnerSee, bOwnerNoSee flags deal with visibility.
		FName AttachPoint = MyPawn->GetWeaponAttachPoint();
		if (MyPawn->IsLocallyControlled() == true)
		{
			USkeletalMeshComponent* PawnMesh1p = MyPawn->GetSpecifcPawnMesh(true);
			USkeletalMeshComponent* PawnMesh3p = MyPawn->GetSpecifcPawnMesh(false);
			Mesh1P->SetHiddenInGame(false);
			Mesh3P->SetHiddenInGame(false);
			Mesh1P->AttachToComponent(PawnMesh1p, FAttachmentTransformRules::KeepRelativeTransform, AttachPoint);
			Mesh3P->AttachToComponent(PawnMesh3p, FAttachmentTransformRules::KeepRelativeTransform, AttachPoint);
			Mesh1P->SetRelativeTransform(WeaponSocketTransform);
			Mesh3P->SetRelativeTransform(WeaponSocketTransform);
		}
		else
		{
			USkeletalMeshComponent* UseWeaponMesh = GetWeaponMesh();
			USkeletalMeshComponent* UsePawnMesh = MyPawn->GetPawnMesh();
			UseWeaponMesh->AttachToComponent(UsePawnMesh, FAttachmentTransformRules::KeepRelativeTransform, AttachPoint);
			UseWeaponMesh->SetHiddenInGame(false);
		}
	}
}

void ANexosWeaponBase::DetachMeshFromPawn()
{
	Mesh1P->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
	Mesh1P->SetHiddenInGame(true);
	Mesh3P->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
	Mesh3P->SetHiddenInGame(true);
}

//////////////////////////////////////////////////////////////////////////
// Input

void ANexosWeaponBase::StartFire()
{
	if (!bTriggerPulled)
	{
		if (GetLocalRole() < ROLE_Authority)
		{
			ServerStartFire();
		}

		if (!bWantsToFire)
		{
			bWantsToFire = true;
			DetermineWeaponState();
		}
	}
}

void ANexosWeaponBase::StopFire()
{
	if (!bBurstFiring)
	{
		if ((GetLocalRole() < ROLE_Authority) && MyPawn && MyPawn->IsLocallyControlled())
		{
			ServerStopFire();
		}

		if (bWantsToFire)
		{
			bWantsToFire = false;
			ShotsFired = 1;
			DetermineWeaponState();
		}
	}
}

void ANexosWeaponBase::StartReload(bool bFromReplication)
{
	if (!bFromReplication && GetLocalRole() < ROLE_Authority)
	{
		ServerStartReload();
	}

	if (bFromReplication || CanReload())
	{
		bPendingReload = true;
		DetermineWeaponState();

		if (WeaponConfig.bIndividualReload)
		{
			FirstLoop = true;
			float AnimDuration = PlayWeaponAnimation(ReloadStart);
			if (AnimDuration <= 0.0f)
			{
				AnimDuration = WeaponConfig.NoAnimReloadTime;
			}
			GetWorldTimerManager().SetTimer(TimerHandle_ReloadWait, this, &ANexosWeaponBase::LoopReload, AnimDuration - 0.1f, false);
		}
		else
		{
			float AnimDuration = PlayWeaponAnimation(ReloadAnim);
			if (AnimDuration <= 0.0f)
			{
				AnimDuration = WeaponConfig.NoAnimReloadTime;
			}
			else
			{
				Mesh1P->PlayAnimation(WeaponReloadAnim, false);
				Mesh3P->PlayAnimation(WeaponReloadAnim, false);
			}

			GetWorldTimerManager().SetTimer(TimerHandle_StopReload, this, &ANexosWeaponBase::StopReload, FMath::Max(0.1f, AnimDuration - 0.1f), false);
			if (GetLocalRole() == ROLE_Authority)
			{
				GetWorldTimerManager().SetTimer(TimerHandle_ReloadWeapon, this, &ANexosWeaponBase::ReloadWeapon, FMath::Max(0.1f, AnimDuration - 0.1f), false);
			}
			if (MyPawn && MyPawn->IsLocallyControlled())
			{
				PlayWeaponSound(ReloadSound);
			}
		}
	}
}

void ANexosWeaponBase::LoopReload()
{
	if(WeaponConfig.AmmoPerClip > CurrentAmmoInClip && CurrentAmmo > 0)
	{
		float AnimLoopDuration = PlayWeaponAnimation(ReloadLoop);
		if (AnimLoopDuration <= 0.0f)
		{
			AnimLoopDuration = WeaponConfig.NoAnimReloadTime;
		}
		else
		{
			Mesh1P->PlayAnimation(WeaponReloadAnim, false);
			Mesh3P->PlayAnimation(WeaponReloadAnim, false);
		}
		GetWorldTimerManager().SetTimer(TimerHandle_ReloadLoop, this, &ANexosWeaponBase::LoopReload, FMath::Max(0.1f, AnimLoopDuration - 0.1f), false);


		if (GetLocalRole() == ROLE_Authority)
		{
			ReloadWeapon();
		}

		if (MyPawn && MyPawn->IsLocallyControlled())
		{
			PlayWeaponSound(ReloadSound);
		}

	}
	else
	{
		GetWorldTimerManager().ClearTimer(TimerHandle_ReloadLoop);
		float StopAnim = PlayWeaponAnimation(ReloadEnd);
		GetWorldTimerManager().SetTimer(TimerHandle_StopReload, this, &ANexosWeaponBase::StopReload, FMath::Max(0.1f, StopAnim - 0.1f), false);
	}
}

void ANexosWeaponBase::StopReload()
{
	if (CurrentState == EWeaponState::Reloading)
	{
		bPendingReload = false;
		DetermineWeaponState();
		StopWeaponAnimation(ReloadAnim);
	}
}

bool ANexosWeaponBase::ServerStartFire_Validate()
{
	return true;
}

void ANexosWeaponBase::ServerStartFire_Implementation()
{
	StartFire();
}

bool ANexosWeaponBase::ServerStopFire_Validate()
{
	return true;
}

void ANexosWeaponBase::ServerStopFire_Implementation()
{
	StopFire();
}

bool ANexosWeaponBase::ServerStartReload_Validate()
{
	return true;
}

void ANexosWeaponBase::ServerStartReload_Implementation()
{
	StartReload();
}

bool ANexosWeaponBase::ServerStopReload_Validate()
{
	return true;
}

void ANexosWeaponBase::ServerStopReload_Implementation()
{
	StopReload();
}

void ANexosWeaponBase::ClientStartReload_Implementation()
{
	StartReload();
}

//////////////////////////////////////////////////////////////////////////
// Control

bool ANexosWeaponBase::CanFire() const
{
	bool bCanFire = MyPawn && MyPawn->CanFire();
	bool bStateOKToFire = ((CurrentState == EWeaponState::Idle) || (CurrentState == EWeaponState::Firing));
	return ((bCanFire == true) && (bStateOKToFire == true) && (bPendingReload == false));
}

bool ANexosWeaponBase::CanReload() const
{
	bool bCanReload = (!MyPawn || MyPawn->CanReload());
	bool bGotAmmo = (CurrentAmmoInClip < WeaponConfig.AmmoPerClip && CurrentAmmo > 0);
	bool bStateOKToReload = ((CurrentState == EWeaponState::Idle) || (CurrentState == EWeaponState::Firing));
	return ((bCanReload == true) && (bGotAmmo == true) && (bStateOKToReload == true));
}

//////////////////////////////////////////////////////////////////////////
// Weapon usage

void ANexosWeaponBase::GiveAmmo(int AddAmount)
{
	const int32 MissingAmmo = FMath::Max(0, WeaponConfig.MaxAmmo - CurrentAmmo);
	AddAmount = FMath::Min(AddAmount, MissingAmmo);
	CurrentAmmo += AddAmount;

	// start reload if clip was empty
	if (GetCurrentAmmoInClip() <= 0 && CanReload() && MyPawn && (MyPawn->GetWeapon() == this))
	{
		ClientStartReload();
	}
}

void ANexosWeaponBase::UseAmmo()
{

	CurrentAmmoInClip--;

	ANexosPlayerController* PlayerController = MyPawn ? Cast<ANexosPlayerController>(MyPawn->GetController()) : NULL;
	ANexosPlayerState* PlayerState = Cast<ANexosPlayerState>(PlayerController->PlayerState);
	switch (GetAmmoType())
	{
	case EAmmoType::EPrimaryAmmo:
		PlayerState;
		break;
	case EAmmoType::ESpecialAmmo:
		PlayerState;
		break;
	case EAmmoType::EHeavyAmmo:
		PlayerState;
		break;
	case EAmmoType::ESmallAmmo:
		PlayerState;
		break;
	}
}

void ANexosWeaponBase::HandleReFiring()
{

	// Update TimerIntervalAdjustment
	UWorld* MyWorld = GetWorld();

	float SlackTimeThisFrame = FMath::Max(0.0f, (MyWorld->TimeSeconds - LastFireTime) - WeaponConfig.FireRate);

	if (bAllowAutomaticWeaponCatchup)
	{
		TimerIntervalAdjustment -= SlackTimeThisFrame;
	}

	HandleFiring();
}

void ANexosWeaponBase::HandleFiring()
{
	switch (FireMode)
	{
	case EFireMode::EAuto:
		HandleAutoFiring();
		break;

	case EFireMode::EBurst:
		HandleBurstFiring();
		break;

	case EFireMode::ESingle:
		HandleSingleFiring();
		break;
	}
}

void ANexosWeaponBase::HandleAutoFiring()
{
	if (CurrentAmmoInClip > 0 && CanFire())
	{
		if (GetNetMode() != NM_DedicatedServer)
		{
			SimulateWeaponFire();
		}

		if (MyPawn && MyPawn->IsLocallyControlled())
		{
			if (WeaponType == EWeaponType::EShotgun)
			{
				for (int32 TraceCount = 0; TraceCount <= WeaponConfig.SlugCount; TraceCount++)
					FireWeapon();
				UseAmmo();
				Mesh1P->PlayAnimation(WeaponFire, false);
				Mesh3P->PlayAnimation(WeaponFire, false);
			}
			else
			{
				FireWeapon();
				UseAmmo();
				Mesh1P->PlayAnimation(WeaponFire, false);
				Mesh3P->PlayAnimation(WeaponFire, false);
			}
			// update firing FX on remote clients if function was called on server
			BurstCounter++;
		}
	}
	else if (CanReload())
	{
		StartReload();
	}
	else if (MyPawn && MyPawn->IsLocallyControlled())
	{
		if (GetCurrentAmmo() == 0 && !bRefiring)
		{
			PlayWeaponSound(OutOfAmmoSound);
			ANexosPlayerController* MyPC = Cast<ANexosPlayerController>(MyPawn->Controller);
			ANexosHUD* MyHUD = MyPC ? Cast<ANexosHUD>(MyPC->GetHUD()) : NULL;
			if (MyHUD)
			{
				MyHUD->NotifyOutOfAmmo();
			}
		}

		// Stop weapon fx (but stay in firing state)
		if (BurstCounter > 0)
		{
			OnBurstFinished();
		}
	}
	else
	{
		OnBurstFinished();
	}

	if (MyPawn && MyPawn->IsLocallyControlled())
	{
		// local client notifys server
		if (GetLocalRole() < ROLE_Authority)
		{
			ServerHandleFiring();
		}

		// reload afer firing last round
		if (CurrentAmmoInClip <= 0 && CanReload())
		{
			StartReload();
		}

		// Setup refire timer
		bRefiring = (CurrentState == EWeaponState::Firing && WeaponConfig.FireRate > 0.0f);
		if (bRefiring)
		{
			GetWorldTimerManager().SetTimer(TimerHandle_HandleAutoFiring, this, &ANexosWeaponBase::HandleReFiring, FMath::Max<float>(WeaponConfig.FireRate + TimerIntervalAdjustment, SMALL_NUMBER), false);
			TimerIntervalAdjustment = 0.f;
		}
	}

	LastFireTime = GetWorld()->GetTimeSeconds();
}

void ANexosWeaponBase::HandleBurstFiring()
{
	bTriggerPulled = true;
	bBurstFiring = true;
	if (CurrentAmmoInClip > 0 && CanFire())
	{
		if (GetNetMode() != NM_DedicatedServer)
		{
			SimulateWeaponFire();
		}

		if (MyPawn && MyPawn->IsLocallyControlled())
		{
			if (WeaponType == EWeaponType::EShotgun)
			{
				for (int32 TraceCount = 0; TraceCount <= WeaponConfig.SlugCount; TraceCount++)
					FireWeapon();
				UseAmmo();
				Mesh1P->PlayAnimation(WeaponFire, false);
				Mesh3P->PlayAnimation(WeaponFire, false);
			}
			else
			{
				FireWeapon();
				UseAmmo();
				Mesh1P->PlayAnimation(WeaponFire, false);
				Mesh3P->PlayAnimation(WeaponFire, false);
			}
			// update firing FX on remote clients if function was called on server
			BurstCounter++;
		}
	}
	else if (CanReload())
	{
		StartReload();
	}
	else if (MyPawn && MyPawn->IsLocallyControlled())
	{
		if (GetCurrentAmmo() == 0 && !bRefiring)
		{
			PlayWeaponSound(OutOfAmmoSound);
			ANexosPlayerController* MyPC = Cast<ANexosPlayerController>(MyPawn->Controller);
			ANexosHUD* MyHUD = MyPC ? Cast<ANexosHUD>(MyPC->GetHUD()) : NULL;
			if (MyHUD)
			{
				MyHUD->NotifyOutOfAmmo();
			}
		}

		// Stop weapon fx (but stay in firing state)
		if (BurstCounter > 0)
		{
			OnBurstFinished();
		}
	}
	else
	{
		OnBurstFinished();
	}

	if (MyPawn && MyPawn->IsLocallyControlled())
	{
		// local client notifys server
		if (GetLocalRole() < ROLE_Authority)
		{
			ServerHandleFiring();
		}

		// reload afer firing last round
		if (CurrentAmmoInClip <= 0 && CanReload())
		{
			StartReload();
		}

		if (ShotsFired < WeaponConfig.BurstFireCount)
		{
			// Setup refire timer
			bRefiring = (CurrentState == EWeaponState::Firing && WeaponConfig.FireRate > 0.0f);
			if (bRefiring)
			{
				GetWorldTimerManager().SetTimer(TimerHandle_HandleAutoFiring, this, &ANexosWeaponBase::HandleReFiring, FMath::Max<float>(WeaponConfig.FireRate + TimerIntervalAdjustment, SMALL_NUMBER), false);
				TimerIntervalAdjustment = 0.f;
				ShotsFired++;
			}
		}
		else
		{
			bBurstFiring = false;
			StopFire();
			GetWorldTimerManager().SetTimer(TimerHandle_ResetTriggerTimer, this, &ANexosWeaponBase::ResetTrigger, FMath::Max<float>(WeaponConfig.BusrtCooldown + TimerIntervalAdjustment, SMALL_NUMBER), false);
		}
	}

	LastFireTime = GetWorld()->GetTimeSeconds();
}

void ANexosWeaponBase::HandleSingleFiring()
{
	bTriggerPulled = true;
	bBurstFiring = true;
	if (CurrentAmmoInClip > 0 && CanFire())
	{
		if (GetNetMode() != NM_DedicatedServer)
		{
			SimulateWeaponFire();
		}

		if (MyPawn && MyPawn->IsLocallyControlled())
		{
			if (WeaponType == EWeaponType::EShotgun)
			{
				for (int32 TraceCount = 0; TraceCount <= WeaponConfig.SlugCount; TraceCount++)
					FireWeapon();
				UseAmmo();
				Mesh1P->PlayAnimation(WeaponFire, false);
				Mesh3P->PlayAnimation(WeaponFire, false);
			}
			else
			{
				FireWeapon();
				UseAmmo();
				Mesh1P->PlayAnimation(WeaponFire, false);
				Mesh3P->PlayAnimation(WeaponFire, false);
			}
			// update firing FX on remote clients if function was called on server
			BurstCounter++;
		}
	}
	else if (CanReload())
	{
		StartReload();
	}
	else if (MyPawn && MyPawn->IsLocallyControlled())
	{
		if (GetCurrentAmmo() == 0 && !bRefiring)
		{
			PlayWeaponSound(OutOfAmmoSound);
			ANexosPlayerController* MyPC = Cast<ANexosPlayerController>(MyPawn->Controller);
			ANexosHUD* MyHUD = MyPC ? Cast<ANexosHUD>(MyPC->GetHUD()) : NULL;
			if (MyHUD)
			{
				MyHUD->NotifyOutOfAmmo();
			}
		}

		// Stop weapon fx (but stay in firing state)
		if (BurstCounter > 0)
		{
			OnBurstFinished();
		}
	}
	else
	{
		OnBurstFinished();
	}

	if (MyPawn && MyPawn->IsLocallyControlled())
	{
		// local client notifys server
		if (GetLocalRole() < ROLE_Authority)
		{
			ServerHandleFiring();
		}

		// reload afer firing last round
		if (CurrentAmmoInClip <= 0 && CanReload())
		{
			StartReload();
		}

		if (ShotsFired < 1)
		{
			// Setup refire timer
			bRefiring = (CurrentState == EWeaponState::Firing && WeaponConfig.FireRate > 0.0f);
			if (bRefiring)
			{
				GetWorldTimerManager().SetTimer(TimerHandle_HandleAutoFiring, this, &ANexosWeaponBase::HandleReFiring, FMath::Max<float>(WeaponConfig.FireRate + TimerIntervalAdjustment, SMALL_NUMBER), false);
				TimerIntervalAdjustment = 0.f;
				ShotsFired++;
			}
		}
		else
		{
			bBurstFiring = false;
			StopFire();
			GetWorldTimerManager().SetTimer(TimerHandle_ResetTriggerTimer, this, &ANexosWeaponBase::ResetTrigger, FMath::Max<float>(WeaponConfig.BusrtCooldown + TimerIntervalAdjustment, SMALL_NUMBER), false);
		}
	}

	LastFireTime = GetWorld()->GetTimeSeconds();
}



void ANexosWeaponBase::ResetTrigger()
{
	bTriggerPulled = false;
	GetWorldTimerManager().ClearTimer(TimerHandle_ResetTriggerTimer);
}


bool ANexosWeaponBase::ServerHandleFiring_Validate()
{
	return true;
}

void ANexosWeaponBase::ServerHandleFiring_Implementation()
{
	const bool bShouldUpdateAmmo = (CurrentAmmoInClip > 0 && CanFire());

	HandleFiring();

	if (bShouldUpdateAmmo)
	{

		// update ammo
		UseAmmo();

		// update firing FX on remote
		BurstCounter++;
	}
}

void ANexosWeaponBase::ReloadWeapon()
{
	if (WeaponConfig.bIndividualReload)
	{
		if(WeaponConfig.AmmoPerClip > CurrentAmmoInClip && CurrentAmmo > 0)
		{
			CurrentAmmoInClip++;

			if (!WeaponConfig.bInfiniteAmmo)
			{
				CurrentAmmo--;
			}
		}
	}
	else
	{
		int32 ClipDelta = FMath::Clamp(WeaponConfig.AmmoPerClip - CurrentAmmoInClip, 0, CurrentAmmo);

		if (ClipDelta > 0)
		{
			CurrentAmmoInClip += ClipDelta;
			if (!WeaponConfig.bInfiniteAmmo)
			{
				CurrentAmmo -= ClipDelta;
			}
		}
	}
}

void ANexosWeaponBase::SetWeaponState(EWeaponState::Type NewState)
{
	const EWeaponState::Type PrevState = CurrentState;

	if (PrevState == EWeaponState::Firing && NewState != EWeaponState::Firing)
	{
		OnBurstFinished();
	}

	CurrentState = NewState;

	if (PrevState != EWeaponState::Firing && NewState == EWeaponState::Firing)
	{
		OnBurstStarted();
	}
}

void ANexosWeaponBase::DetermineWeaponState()
{
	EWeaponState::Type NewState = EWeaponState::Idle;

	if (bIsEquipped)
	{
		if (bPendingReload)
		{
			if (CanReload() == false)
			{
				NewState = CurrentState;
			}
			else
			{
				NewState = EWeaponState::Reloading;
			}
		}
		else if ((bPendingReload == false) && (bWantsToFire == true) && (CanFire() == true))
		{
			NewState = EWeaponState::Firing;
		}
	}
	else if (bPendingEquip)
	{
		NewState = EWeaponState::Equipping;
	}

	SetWeaponState(NewState);
}

void ANexosWeaponBase::OnBurstStarted()
{
	// start firing, can be delayed to satisfy TimeBetweenShots
	const float GameTime = GetWorld()->GetTimeSeconds();
	if (LastFireTime > 0 && WeaponConfig.FireRate > 0.0f &&
		LastFireTime + WeaponConfig.FireRate > GameTime)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_HandleAutoFiring, this, &ANexosWeaponBase::HandleFiring, LastFireTime + WeaponConfig.FireRate - GameTime, false);
	}
	else
	{
		HandleFiring();
	}
}

void ANexosWeaponBase::OnBurstFinished()
{
	// stop firing FX on remote clients
	BurstCounter = 0;

	// stop firing FX locally, unless it's a dedicated server
	//if (GetNetMode() != NM_DedicatedServer)
	//{
	StopSimulatingWeaponFire();
	//}

	GetWorldTimerManager().ClearTimer(TimerHandle_HandleAutoFiring);
	GetWorldTimerManager().ClearTimer(TimerHandle_HandleBurstFiring);
	GetWorldTimerManager().ClearTimer(TimerHandle_HandleSingleFiring);
	bRefiring = false;

	// reset firing interval adjustment
	TimerIntervalAdjustment = 0.0f;
}


//////////////////////////////////////////////////////////////////////////
// Weapon usage helpers

UAudioComponent* ANexosWeaponBase::PlayWeaponSound(USoundCue* Sound)
{
	UAudioComponent* AC = NULL;
	if (Sound && MyPawn)
	{
		AC = UGameplayStatics::SpawnSoundAttached(Sound, MyPawn->GetRootComponent());
	}

	return AC;
}

float ANexosWeaponBase::PlayWeaponAnimation(const FWeaponAnim& Animation)
{
	float Duration = 0.0f;
	if (MyPawn)
	{
		UAnimMontage* UseAnim = MyPawn->IsFirstPerson() ? Animation.Pawn1P : Animation.Pawn3P;
		if (UseAnim)
		{
			Duration = MyPawn->PlayAnimMontage(UseAnim);
		}
	}

	return Duration;
}

void ANexosWeaponBase::StopWeaponAnimation(const FWeaponAnim& Animation)
{
	if (MyPawn)
	{
		UAnimMontage* UseAnim = MyPawn->IsFirstPerson() ? Animation.Pawn1P : Animation.Pawn3P;
		if (UseAnim)
		{
			MyPawn->StopAnimMontage(UseAnim);
		}
	
	}
}

FVector ANexosWeaponBase::GetCameraAim() const
{
	ANexosPlayerController* const PlayerController = GetInstigatorController<ANexosPlayerController>();
	FVector FinalAim = FVector::ZeroVector;

	if (PlayerController)
	{
		FVector CamLoc;
		FRotator CamRot;
		PlayerController->GetPlayerViewPoint(CamLoc, CamRot);
		FinalAim = CamRot.Vector();
	}
	else if (GetInstigator())
	{
		FinalAim = GetInstigator()->GetBaseAimRotation().Vector();
	}

	return FinalAim;
}

FVector ANexosWeaponBase::GetAdjustedAim() const
{
	ANexosPlayerController* const PlayerController = GetInstigatorController<ANexosPlayerController>();
	FVector FinalAim = FVector::ZeroVector;
	// If we have a player controller use it for the aim
	if (PlayerController)
	{
		FVector CamLoc;
		FRotator CamRot;
		PlayerController->GetPlayerViewPoint(CamLoc, CamRot);
		FinalAim = CamRot.Vector();
	}

	return FinalAim;
}

FVector ANexosWeaponBase::GetCameraDamageStartLocation(const FVector& AimDir) const
{
	ANexosPlayerController* PC = MyPawn ? Cast<ANexosPlayerController>(MyPawn->Controller) : NULL;
	FVector OutStartTrace = FVector::ZeroVector;

	if (PC)
	{
		// use player's camera
		FRotator UnusedRot;
		PC->GetPlayerViewPoint(OutStartTrace, UnusedRot);

		// Adjust trace so there is nothing blocking the ray between the camera and the pawn, and calculate distance from adjusted start
		OutStartTrace = OutStartTrace + AimDir * ((GetInstigator()->GetActorLocation() - OutStartTrace) | AimDir);
	}

	return OutStartTrace;
}

FVector ANexosWeaponBase::GetMuzzleLocation() const
{
	USkeletalMeshComponent* UseMesh = GetWeaponMesh();
	return UseMesh->GetSocketLocation(MuzzleAttachPoint);
}

FVector ANexosWeaponBase::GetMuzzleDirection() const
{
	USkeletalMeshComponent* UseMesh = GetWeaponMesh();
	return UseMesh->GetSocketRotation(MuzzleAttachPoint).Vector();
}

FHitResult ANexosWeaponBase::WeaponTrace(const FVector& StartTrace, const FVector& EndTrace) const
{

	// Perform trace to retrieve hit info
	FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(WeaponTrace), true, GetInstigator());
	TraceParams.bReturnPhysicalMaterial = true;

	FHitResult Hit(ForceInit);
	GetWorld()->LineTraceSingleByChannel(Hit, StartTrace, EndTrace, COLLISION_WEAPON, TraceParams);

	return Hit;
}

void ANexosWeaponBase::SetOwningPawn(ANexosPlayerCharacter* NewOwner)
{
	if (MyPawn != NewOwner)
	{
		SetInstigator(NewOwner);
		MyPawn = NewOwner;
		// net owner for RPC calls
		SetOwner(NewOwner);
	}
}

//////////////////////////////////////////////////////////////////////////
// Replication & effects

void ANexosWeaponBase::OnRep_MyPawn()
{
	if (MyPawn)
	{
		OnEnterInventory(MyPawn);
	}
	else
	{
		OnLeaveInventory();
	}
}

void ANexosWeaponBase::OnRep_BurstCounter()
{
	if (BurstCounter > 0)
	{
		SimulateWeaponFire();
	}
	else
	{
		StopSimulatingWeaponFire();
	}
}

void ANexosWeaponBase::OnRep_Reload()
{
	if (bPendingReload)
	{
		StartReload(true);
	}
	else
	{
		StopReload();
	}
}

void ANexosWeaponBase::SimulateWeaponFire()
{
	if (GetLocalRole() == ROLE_Authority && CurrentState != EWeaponState::Firing)
	{
		return;
	}

	if (MuzzleFX)
	{
		USkeletalMeshComponent* UseWeaponMesh = GetWeaponMesh();
		if (!bLoopedMuzzleFX || MuzzlePSC == NULL)
		{
			// Split screen requires we create 2 effects. One that we see and one that the other player sees.
			if ((MyPawn != NULL) && (MyPawn->IsLocallyControlled() == true))
			{
				AController* PlayerCon = MyPawn->GetController();
				if (PlayerCon != NULL)
				{
					Mesh1P->GetSocketLocation(MuzzleAttachPoint);
					MuzzlePSC = UGameplayStatics::SpawnEmitterAttached(MuzzleFX, Mesh1P, MuzzleAttachPoint);
					MuzzlePSC->bOwnerNoSee = false;
					MuzzlePSC->bOnlyOwnerSee = true;

					Mesh3P->GetSocketLocation(MuzzleAttachPoint);
					MuzzlePSCSecondary = UGameplayStatics::SpawnEmitterAttached(MuzzleFX, Mesh3P, MuzzleAttachPoint);
					MuzzlePSCSecondary->bOwnerNoSee = true;
					MuzzlePSCSecondary->bOnlyOwnerSee = false;
				}
			}
			else
			{
				MuzzlePSC = UGameplayStatics::SpawnEmitterAttached(MuzzleFX, UseWeaponMesh, MuzzleAttachPoint);
			}
		}
	}

	if (!bLoopedFireAnim || !bPlayingFireAnim)
	{
		PlayWeaponAnimation(FireAnim);
		bPlayingFireAnim = true;
	}

	if (bLoopedFireSound)
	{
		if (FireAC == NULL)
		{
			FireAC = PlayWeaponSound(FireLoopSound);
		}
	}
	else
	{
		PlayWeaponSound(FireSound);
	}

	ANexosPlayerController* PC = (MyPawn != NULL) ? Cast<ANexosPlayerController>(MyPawn->Controller) : NULL;
	if (PC != NULL && PC->IsLocalController())
	{
		if (FireCameraShake != NULL)
		{
			PC->ClientStartCameraShake(FireCameraShake, 1);
		}
		if (FireForceFeedback != NULL && PC->IsVibrationEnabled())
		{
			FForceFeedbackParameters FFParams;
			FFParams.Tag = "Weapon";
			PC->ClientPlayForceFeedback(FireForceFeedback, FFParams);
		}
	}
}

void ANexosWeaponBase::StopSimulatingWeaponFire()
{
	if (bLoopedMuzzleFX)
	{
		if (MuzzlePSC != NULL)
		{
			MuzzlePSC->DeactivateSystem();
			MuzzlePSC = NULL;
		}
		if (MuzzlePSCSecondary != NULL)
		{
			MuzzlePSCSecondary->DeactivateSystem();
			MuzzlePSCSecondary = NULL;
		}
	}

	if (bLoopedFireAnim && bPlayingFireAnim)
	{
		StopWeaponAnimation(FireAnim);
		bPlayingFireAnim = false;
	}

	if (FireAC)
	{
		FireAC->FadeOut(0.1f, 0.0f);
		FireAC = NULL;

		PlayWeaponSound(FireFinishSound);
	}
}


void ANexosWeaponBase::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ANexosWeaponBase, MyPawn);

	DOREPLIFETIME_CONDITION(ANexosWeaponBase, CurrentAmmo, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ANexosWeaponBase, CurrentAmmoInClip, COND_OwnerOnly);

	DOREPLIFETIME_CONDITION(ANexosWeaponBase, BurstCounter, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(ANexosWeaponBase, bPendingReload, COND_SkipOwner);
}

USkeletalMeshComponent* ANexosWeaponBase::GetWeaponMesh() const
{
	return (MyPawn != NULL && MyPawn->IsFirstPerson()) ? Mesh1P : Mesh3P;
}

class ANexosPlayerCharacter* ANexosWeaponBase::GetPawnOwner() const
{
	return MyPawn;
}


bool ANexosWeaponBase::IsEquipped() const
{
	return bIsEquipped;
}

bool ANexosWeaponBase::IsAttachedToPawn() const
{
	return bIsEquipped || bPendingEquip;
}

EWeaponState::Type ANexosWeaponBase::GetCurrentState() const
{
	return CurrentState;
}

int32 ANexosWeaponBase::GetCurrentAmmo() const
{
	return CurrentAmmo;
}

int32 ANexosWeaponBase::GetCurrentAmmoInClip() const
{
	return CurrentAmmoInClip;
}

int32 ANexosWeaponBase::GetAmmoPerClip() const
{
	return WeaponConfig.AmmoPerClip;
}

int32 ANexosWeaponBase::GetMaxAmmo() const
{
	return WeaponConfig.MaxAmmo;
}

bool ANexosWeaponBase::HasInfiniteAmmo() const
{
	const ANexosPlayerController* MyPC = (MyPawn != NULL) ? Cast<const ANexosPlayerController>(MyPawn->Controller) : NULL;
	return WeaponConfig.bInfiniteAmmo || (MyPC && MyPC->HasInfiniteAmmo());
}


float ANexosWeaponBase::GetEquipStartedTime() const
{
	return EquipStartedTime;
}

float ANexosWeaponBase::GetEquipDuration() const
{
	return EquipDuration;
}

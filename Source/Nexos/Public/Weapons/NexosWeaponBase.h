#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/Canvas.h"
#include "Camera/CameraShake.h"
#include "NexosWeaponBase.generated.h"

class UAnimMontage;
class ANexosPlayerCharacter;
class UAudioComponent;
class UParticleSystemComponent;

class UForceFeedbackEffect;
class USoundCue;

namespace EWeaponState
{
	enum Type
	{
		Idle,
		Firing,
		Reloading,
		Equipping,
	};
}


UENUM(BlueprintType)
enum class EAmmoType : uint8
{
	EPrimaryAmmo	 UMETA(DisplayName = "Primary"),
	ESpecialAmmo	 UMETA(DisplayName = "Special"),
	EHeavyAmmo	 UMETA(DisplayName = "Heavy"),
	ESmallAmmo	 UMETA(DisplayName = "Small"),
};

/** Element type */
UENUM(BlueprintType)
enum class EElement : uint8
{
	EForce	 UMETA(DisplayName = "Force"),
	EGrav	 UMETA(DisplayName = "Gravaton"),
	ENutr	 UMETA(DisplayName = "Neutron"),
	EElec	 UMETA(DisplayName = "Electron"),
};

/** Fire mode */
UENUM(BlueprintType)
enum class EFireMode : uint8
{
	ESingle	 UMETA(DisplayName = "Single"),
	EBurst	 UMETA(DisplayName = "Burst"),
	EAuto	 UMETA(DisplayName = "Auto"),
};

/** What Weapon Slot is this equiped to */
UENUM(BlueprintType)
enum class EWeaponSlot : uint8
{
	EPrimary	 UMETA(DisplayName = "Primary"),
	ESpecial	 UMETA(DisplayName = "Special"),
	EHeavy	 UMETA(DisplayName = "Heavy"),
	ESidearm	 UMETA(DisplayName = "Sidearm"),
};

/** What Weapon type is this equiped to */
UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EAssaultRifle		UMETA(DisplayName = "Assault Rifle"),
	EBurstRifle			UMETA(DisplayName = "Burst Rifle"),
	EMarksmenRifle		UMETA(DisplayName = "Marksman Rifle"),
	EHandCannon			UMETA(DisplayName = "Hand Cannon"),
	ESMG				UMETA(DisplayName = "SMG"),
	ESniperRifle		UMETA(DisplayName = "Sniper Rifle"),
	EShotgun			UMETA(DisplayName = "Shotgun"),
	EParticleRifle		UMETA(DisplayName = "Particle Rifle"),
	ERailgun			UMETA(DiaplayName = "Railgun"),
	ERocketLauncher		UMETA(DisplayName = "Rocket Launcher"),
	EGrenadeLauncher	UMETA(DisplayName = "Grenade Launcher"),
	ESword				UMETA(DisplayName = "Sword"),
	EHandgun			UMETA(DisplayName = "Handgun"),
	EMiniSMG			UMETA(DisplayName = "Mini SMG"),
};

USTRUCT()
struct FWeaponData
{
	GENERATED_USTRUCT_BODY()

	/** should this weapon have infinite ammo*/
	UPROPERTY(EditDefaultsOnly, Category = "Ammo")
		bool bInfiniteAmmo;

	/** Is this a melee weapon? if so we dont want any ammo */
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Class")
		bool bIsMeleeWeapon;

	/** Is this a melee weapon? if so we dont want any ammo */
    UPROPERTY(EditDefaultsOnly, Category = "Weapon Class")
		bool bIsSidearmWeapon;
	
	/** how many slugs/pellets to fire if Shotgun/Scattergun */
	UPROPERTY(EditDefaultsOnly, Category = "Ammo")
		bool bIndividualReload;

	UPROPERTY(EditDefaultsOnly, Category = "Name")
		FString WeaponName;

	/**Maximum ammo that a weapon can hold (Clip and Reserves) */
	UPROPERTY(EditDefaultsOnly, Category = "Ammo")
		int32 MaxAmmo;

	/** How much ammo is in a given clip */
	UPROPERTY(EditAnywhere, Category = "Ammo")
		int32 AmmoPerClip;

	/** How much ammo is given at initial spawn */
	UPROPERTY(EditDefaultsOnly, Category = "Ammo")
		int32 InitialAmmo;

	/** How much ammo is given at initial spawn */
	UPROPERTY(EditDefaultsOnly, Category = "Ammo")
		int32 BurstFireCount;

	/** how many slugs/pellets to fire if Shotgun/Scattergun */
	UPROPERTY(EditDefaultsOnly, Category = "Ammo")
		int32 SlugCount;

	/** How much of a delay is between shots */
	UPROPERTY(EditDefaultsOnly, Category = "WeaponStats")
		float FireRate;

	UPROPERTY(EditDefaultsOnly, Category = "WeaponStats")
		float BusrtCooldown;

	/** Default reload time if there is no animation for it */
	UPROPERTY(EditDefaultsOnly, Category = "WeaponStats")
		float NoAnimReloadTime;

	/** Default Stats, should alwasy be changerd per weapon */
	FWeaponData()
	{
		WeaponName = " ";
		bInfiniteAmmo = false;
		bIndividualReload = false;
		MaxAmmo = 330;
		AmmoPerClip = 30;
		InitialAmmo = 120;
		BurstFireCount = 0;
		SlugCount = 0;
		FireRate = 0.2f;
		BusrtCooldown = 0.0f;
		NoAnimReloadTime = 1.0f;
	}
};

USTRUCT()
struct FWeaponAnim
{
	GENERATED_USTRUCT_BODY()

		/** Animation for 1st person view */
	UPROPERTY(EditDefaultsOnly, Category = "Animations")
		UAnimMontage* Pawn1P;

	/** Animation got 3rd person view */
	UPROPERTY(EditDefaultsOnly, Category = "Animations")
		UAnimMontage* Pawn3P;
};

UCLASS(Abstract, Blueprintable)
class NEXOS_API ANexosWeaponBase : public AActor
{
	GENERATED_UCLASS_BODY()

	virtual void PostInitializeComponents() override;
	virtual void Destroyed() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EAmmoType AmmoType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EElement Element;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EFireMode FireMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EWeaponSlot WeaponSlot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EWeaponType WeaponType;

	/** [server] add ammo to weapon */
	void GiveAmmo(int AddAmmount);

	/** consume ammo */
	void UseAmmo();

	virtual EAmmoType GetAmmoType() const
	{
		return EAmmoType::EPrimaryAmmo;
	}

	//////////////////////////////////////////////////////////////////////////////
	// Inventory Interactions

	/** Weapon is being equip by owner pawn */
	virtual void OnEquip(const ANexosWeaponBase* LastWeapon);

	/** Weapon is now the active weapon for ower pawn */
	virtual void OnEquipFinished();

	/** Weapon is holstered by owner Pawn */
	virtual void OnUnEquip();

	/** [server] weapon is stored in ower pawn inventory */
	virtual void OnEnterInventory(ANexosPlayerCharacter* NewOwner);

	/** [server] When the weapon is removed from the owner pawn inventory */
	virtual void OnLeaveInventory();

	/** check is the weapon is currently equipped */
	bool IsEquipped() const;

	/** check if the mesh is already attached */
	bool IsAttachedToPawn() const;

	///////////////////////////////////////////////////////////////////////////////
	// Inputs

	/** [local + server] start weapon fire */
	virtual void StartFire();

	/** [local + server] stop weapon firing */
	virtual void StopFire();

	/** [all] start weapon reload */
	virtual void StartReload(bool bFromReplication = false);

	/** [all] loop weapon reload */
	virtual void LoopReload();

	bool FirstLoop;

	/** [local + server] interupt reload, this is used for: Sprint, Jump Ability, Momentum Drive, Melee, Grenade, Equipment */
	virtual void StopReload();

	/** [server] performs math for and actualy runs the reload */
	virtual void ReloadWeapon();

	/** triggers the reload from server side */
	UFUNCTION(reliable, client)
		void ClientStartReload();

	///////////////////////////////////////////////////////////////////////////////
	// Controlles 

	/** verify that the weapon can fire */
	bool CanFire() const;

	/** verify that the weapon can reload */
	bool CanReload() const;

	///////////////////////////////////////////////////////////////////////////////
	// Read Data

	/** get current state */
	EWeaponState::Type GetCurrentState() const;

	/** get current total ammo ammoun */

	int32 GetCurrentAmmo() const;

	/** cet current ammount of ammo in clip */
	int32 GetCurrentAmmoInClip() const;

	/** get the clip size */
	UFUNCTION(BlueprintCallable, Category = "Ammo")
	int32 GetAmmoPerClip() const;

	/** get the max ammount of ammo */
	int32 GetMaxAmmo() const;

	/** get weapon mesh (Needs a pawn owner to determin variant needed) */
	USkeletalMeshComponent* GetWeaponMesh() const;

	/** 3rd person view mesh */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
		USkeletalMeshComponent* EmptyMag;

	/** 3rd person view mesh */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
		USkeletalMeshComponent* MagAmmo;

	/** get owning pawn */
	UFUNCTION(BlueprintCallable, Category = "Game|Weapon")
		class ANexosPlayerCharacter* GetPawnOwner() const;

	/** icon to display for the Weapon */
	UPROPERTY(EditDefaultsOnly, Category = "HUD")
		FCanvasIcon WeaponIcon;

	/** Crosshair parts icons (left, top, right, bottom, center) */
	UPROPERTY(EditDefaultsOnly, Category = "HUD")
		FCanvasIcon Crosshair[5];

	/** Crosshair parts icons when ADS (left, top, right, bottom, center) */
	UPROPERTY(EditDefaultsOnly, Category = "HUD")
		FCanvasIcon CrosshairADS[5];

	/** Use a red colored dot in the middle */
	UPROPERTY(EditDefaultsOnly, Category = "HUD")
		bool bUseRedAimDot;

	/** false uses default */
	UPROPERTY(EditDefaultsOnly, Category = "HUD")
		bool bUseCustomCrosshairs;

	/** false uses custom one if set, otherwise uses default */
	UPROPERTY(EditDefaultsOnly, Category = "HUD")
		bool bUseCustomCrosshairADS;

	/** true means crosshair will not be shown unless aiming with the weapon */
	UPROPERTY(EditDefaultsOnly, Category = "HUD")
		bool bHideCrosshairWhileNotAiming;

	/** Adjustment to handle FR affecting acctual timers */
	UPROPERTY(Transient)
		float TimerIntervalAdjustment;

	/** If allow automatic weapons to catchup with shorter delay cycles */
	UPROPERTY(Config)
		bool bAllowAutomaticWeaponCatchup = true;

	/** Check if weapon has infinite ammo */
	bool HasInfiniteAmmo() const;

	/** Set the weapons owning Pawn */
	void SetOwningPawn(ANexosPlayerCharacter* ANexosPlayerCharacter);

	/** Gets the last time this weapon was equipped */
	float GetEquipStartedTime() const;

	/** gets the duration of equipping time */
	float GetEquipDuration() const;

	UPROPERTY(EditDefaultsOnly, Category = "Config")
		FTransform ClipSocketTransform;

	UPROPERTY(EditDefaultsOnly, Category = "Config")
		FTransform WeaponSocketTransform;

protected:

	/** pawn owner */
	UPROPERTY(Transient, ReplicatedUsing = OnRep_MyPawn)
		class ANexosPlayerCharacter* MyPawn;

	/** weapon data */
	UPROPERTY(EditDefaultsOnly, Category = "Config")
		FWeaponData WeaponConfig;



private:

	/** 1st person view mesh */
	UPROPERTY(VisibleDefaultsOnly, Category = "Mesh")
		USkeletalMeshComponent* Mesh1P;

	/** 3rd person view mesh */
	UPROPERTY(VisibleDefaultsOnly, Category = "Mesh")
		USkeletalMeshComponent* Mesh3P;

protected:

	/** Firing audio (bLoopedFireSound set) */
	UPROPERTY(Transient)
		UAudioComponent* FireAC;

	/** name of bone/socket for muzzle in weapon mesh */
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
		FName MuzzleAttachPoint;

	/** FX for muzzle flash */
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
		UParticleSystem* MuzzleFX;

	/** spawned component for muzzle FX */
	UPROPERTY(Transient)
		UParticleSystemComponent* MuzzlePSC;

	/** spawned component for second muzzle FX (Needed for split screen) */
	UPROPERTY(Transient)
		UParticleSystemComponent* MuzzlePSCSecondary;

	/** camera shake on firing */
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TSubclassOf<UMatineeCameraShake> FireCameraShake;

	/** force feedback effect to play when the weapon is fired */
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
		UForceFeedbackEffect* FireForceFeedback;

	/** single fire sound (bLoopedFireSound not set) */
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
		USoundCue* FireSound;

	/** looped fire sound (bLoopedFireSound set) */
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
		USoundCue* FireLoopSound;

	/** finished burst sound (bLoopedFireSound set) */
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
		USoundCue* FireFinishSound;

	/** out of ammo sound */
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
		USoundCue* OutOfAmmoSound;

	/** reload sound */
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
		USoundCue* ReloadSound;

	/** Animation for the Weapon */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animations")
		UAnimMontage* WeaponReloadAnim;

	/** Animation for the Weapon */
	UPROPERTY(EditDefaultsOnly, Category = "Animations")
		UAnimMontage* WeaponFire;

	
	/** Animation for the Weapon */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animations")
		UBlendSpaceBase* WeaponAimOffsets;

	/** equip sound */
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
		USoundCue* EquipSound;

	/** equip animations */
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
		FWeaponAnim ReloadAnim;

	/** equip animations */
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
		FWeaponAnim ReloadStart;

	/** equip animations */
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
		FWeaponAnim ReloadLoop;

	/** equip animations */
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
		FWeaponAnim ReloadEnd;

	/** equip animations */
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
		FWeaponAnim EquipAnim;

	/** fire animations */
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
		FWeaponAnim FireAnim;

	/** is muzzle FX looped? */
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
		uint32 bLoopedMuzzleFX : 1;

	/** is fire sound looped? */
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
		uint32 bLoopedFireSound : 1;

	/** is fire animation looped? */
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
		uint32 bLoopedFireAnim : 1;

	/** is fire animation playing? */
	uint32 bPlayingFireAnim : 1;

	/** is weapon currently equipped? */
	uint32 bIsEquipped : 1;

	/** is weapon fire active? */
	uint32 bWantsToFire : 1;

	uint32 bTriggerPulled : 1;

	uint32 bBurstFiring : 1;

	int ShotsFired = 1;

	/** is reload animation playing? */
	UPROPERTY(Transient, ReplicatedUsing = OnRep_Reload)
		uint32 bPendingReload : 1;

	/** is equip animation playing? */
	uint32 bPendingEquip : 1;

	/** weapon is refiring */
	uint32 bRefiring;

	/** current weapon state */
	EWeaponState::Type CurrentState;

	/** time of last successful weapon fire */
	float LastFireTime;

	/** last time when this weapon was switched to */
	float EquipStartedTime;

	/** how much time weapon needs to be equipped */
	float EquipDuration;

	/** current total ammo */
	UPROPERTY(BlueprintReadWrite, Transient, Replicated)
		int32 CurrentAmmo;

	/** current ammo - inside clip */
	UPROPERTY(BlueprintReadWrite, Transient, Replicated)
		int32 CurrentAmmoInClip;

	/** burst counter, used for replicating fire events to remote clients */
	UPROPERTY(Transient, ReplicatedUsing = OnRep_BurstCounter)
		int32 BurstCounter;

	/** Handle for efficient management of OnEquipFinished timer */
	FTimerHandle TimerHandle_OnEquipFinished;

	/** Handle for efficient management of StopReload timer */
	FTimerHandle TimerHandle_StopReload;

	/** Handle for efficient management of ReloadWeapon timer */
	FTimerHandle TimerHandle_ReloadWeapon;

	/** Handle for efficient management of ReloadWeapon timer */
	FTimerHandle TimerHandle_ReloadLoop;

	FTimerHandle TimerHandle_ReloadWait;

	/** Handle for efficient management of HandleFiring timer */
	FTimerHandle TimerHandle_HandleAutoFiring;

	FTimerHandle TimerHandle_HandleBurstFiring;

	FTimerHandle TimerHandle_HandleSingleFiring;

	FTimerHandle TimerHandle_ResetTriggerTimer;

	//////////////////////////////////////////////////////////////////////////
	// Input - server side

	UFUNCTION(reliable, server, WithValidation)
		void ServerStartFire();

	UFUNCTION(reliable, server, WithValidation)
		void ServerStopFire();

	UFUNCTION(reliable, server, WithValidation)
		void ServerStartReload();

	UFUNCTION(reliable, server, WithValidation)
		void ServerStopReload();


	//////////////////////////////////////////////////////////////////////////
	// Replication & effects

	UFUNCTION()
		void OnRep_MyPawn();

	UFUNCTION()
		void OnRep_BurstCounter();

	UFUNCTION()
		void OnRep_Reload();

	/** Called in network play to do the cosmetic fx for firing */
	virtual void SimulateWeaponFire();

	/** Called in network play to stop cosmetic fx (e.g. for a looping shot). */
	virtual void StopSimulatingWeaponFire();


	//////////////////////////////////////////////////////////////////////////
	// Weapon usage

	/** [local] weapon specific fire implementation */
	virtual void FireWeapon() PURE_VIRTUAL(ANexosWeaponBase::FireWeapon, );

	/** [server] fire & update ammo */
	UFUNCTION(reliable, server, WithValidation)
		void ServerHandleFiring();

	/** [local + server] handle weapon refire, compensating for slack time if the timer can't sample fast enough */
	void HandleReFiring();

	/** [local + server] handle weapon fire */
	void HandleFiring();

	void HandleAutoFiring();
	void HandleBurstFiring();
	void HandleSingleFiring();

	void ResetTrigger();

	/** [local + server] firing started */
	virtual void OnBurstStarted();

	/** [local + server] firing finished */
	virtual void OnBurstFinished();

	/** update weapon state */
	void SetWeaponState(EWeaponState::Type NewState);

	/** determine current weapon state */
	void DetermineWeaponState();


	//////////////////////////////////////////////////////////////////////////
	// Inventory

	/** attaches weapon mesh to pawn's mesh */
	void AttachMeshToPawn();

	/** detaches weapon mesh from pawn */
	void DetachMeshFromPawn();


	//////////////////////////////////////////////////////////////////////////
	// Weapon usage helpers

	/** play weapon sounds */
	UAudioComponent* PlayWeaponSound(USoundCue* Sound);

	/** play weapon animations */
	float PlayWeaponAnimation(const FWeaponAnim& Animation);

	/** stop playing weapon animations */
	void StopWeaponAnimation(const FWeaponAnim& Animation);

	/** Get the aim of the weapon, allowing for adjustments to be made by the weapon */
	virtual FVector GetAdjustedAim() const;

	/** Get the aim of the camera */
	FVector GetCameraAim() const;

	/** get the originating location for camera damage */
	FVector GetCameraDamageStartLocation(const FVector& AimDir) const;

	/** get the muzzle location of the weapon */
	FVector GetMuzzleLocation() const;

	/** get direction of weapon's muzzle */
	FVector GetMuzzleDirection() const;

	/** find hit */
	FHitResult WeaponTrace(const FVector& TraceFrom, const FVector& TraceTo) const;

protected:
	/** Returns Mesh1P subobject **/
	FORCEINLINE USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns Mesh3P subobject **/
	FORCEINLINE USkeletalMeshComponent* GetMesh3P() const { return Mesh3P; }
};


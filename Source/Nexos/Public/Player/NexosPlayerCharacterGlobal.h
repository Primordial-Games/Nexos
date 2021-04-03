/*////////////////////////////////////////////////////////////////////
* Copyright (c) 2021 Primordial Games LLC
* All rights reserved.
*////////////////////////////////////////////////////////////////////

#pragma once

#include "CoreMinimal.h"
#include "Player/NexosPlayerCharacterBase.h"
#include "NexosPlayerCharacterGlobal.generated.h"

/**
 * 
 */

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnNexosPlayerDrawWeapon, ANexosPlayerCharacterGlobal*, ANexosWeaponBase*);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnNexosPlayerStowWeapon, ANexosPlayerCharacterGlobal*, ANexosWeaponBase*);


UCLASS(Abstract)
class NEXOS_API ANexosPlayerCharacterGlobal : public ANexosPlayerCharacterBase
{
	GENERATED_UCLASS_BODY()

	virtual void BeginDestroy() override;

	virtual void PostInitializeComponents() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void Destroyed() override;

	virtual void PawnClientRestart() override;

	virtual bool IsReplicationPausedForConnection(const FNetViewer& ConnectionOwnerNetViewer) override;
	
	virtual void OnReplicationPausedChanged(bool bIsReplicationPaused) override;

	void OnCameraUpdate(const FVector& CameraLocation, const FRotator& CameraRotation);

	UFUNCTION(BlueprintCallable, Category = "Game|Weapon")
		FRotator GetAimOffset() const;

	bool IsEnemyFor(AController* TestPC) const;

	void AddWeaopon(class ANexosWeaponBase* Weapon);

	void RemoveWeapon(class ANexosWeaponBase* Weapon);

	class ANexosWeaponBase* FindWeapon(TSubclassOf<class ANexosWeaponBase> WeaponClass);

	void DrawWeapon(class ANexosWeaponBase* Weapon);

	//////////////////////////////////////////////////////////////////////////
	// Weapon usage

	void StartWeaponFire();

	void StopWeaponFire();

	bool CanFire() const;

	bool CanReload() const;

	void SetTarget(bool bNewTargeting);

	void SetCamera(bool bIsTargetingActive);

	//////////////////////////////////////////////////////////////////////////
	// Movement Abilities

	void SetRunning(bool bNewRunning, bool bToggle);

	void SetJumpAbility(bool bNewJump);

	void SetMomentumDrive(bool bNewMomentumDrive);

	//////////////////////////////////////////////////////////////////////////
	// Animations

	virtual float PlayAnimMontage(UAnimMontage* AnimMontage, float InPlayRate, FName StartSectionName) override;
	virtual void StopAnimMontage(UAnimMontage* AnimMontage) override;

	void StopAllAnimMontages();

	//////////////////////////////////////////////////////////////////////////
	// Inputs

	void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	
	/**
	 *Move Forwards/Backwards
	 */
	void MoveForward(float Val);

	/**
	 *Strafe Left and Right
	 */
	void MoveRight(float Val);

	/**
	 *Move Up and Down in movement Modes that allow it
	 */
	void MoveUp(float Val);

	/* Frame rate independent turn */
	void TurnAtRate(float Val);

	/* Frame rate independent lookup */
	void LookUpAtRate(float Val);

	/** Player begins fire action */
	void OnStartFire();

	/** Player ends fire action */
	void OnStopFire();

	/** PLayer starts aiming */
	void OnStartAiming();

	/** Players ends aiming */
	void OnStopAiming();

	/* player switches to next weapon */
	void OnNextWeapon();

	/* Player switches to previous weapon */
	void OnPrevWeapon();

	void OnSwapWeapons();

	void OnPrimaryWeapon();

	void OnSecondaryWeapon();

	void OnHeavyWeapon();

	void OnSidearmWeapon();

	/** player begins reload */
	void OnReload();

	/** player presses jump button */
	void OnStartJump();

	/** player releases the jump button */
	void OnStopJump();
	
	/** player pressed run action */
	void OnStartRunning();

	/** player pressed toggled run action */
	void OnStartRunningToggle();

	/** player released run action */
	void OnStopRunning();

	/** Player presses Grenade button*/
	void BeginGrenadeThrow();

	/** Player releases Grenade Button */
	void EndGrenadeThrow();

	/** player Presses Melee Button */
	void BeginMelee();

	/** Player releases melee button */
	void EndMelee();

	/** Player Presses tactical button */
	void BeginTactical();

	/** Player releases tactical button */
	void EndTactical();

	/** Player presses Singularity button */
	void BeginSingularity();

	/** Player releases Singularity button */
	void EndSingularity();
	
	/** Player begins using the Momentum Drive */
	void OnStartMomentumDrive();

	/** Player ends using the Momentum Drive */
	void OnStopMomentumDrive();

	void OnStartInteract();

	void OnStopInteract();

	//////////////////////////////////////////////////////////////////////////
	// Reading data

	/** get mesh component */
	USkeletalMeshComponent* GetPawnMesh() const;

	/** Get weapon that is currently drawn */
	UFUNCTION(BlueprintCallable, Category = "Game|Weapon")
		class ANexosWeaponBase* GetWeapon() const;

	/** Get the previous weapon to show on character */
	UFUNCTION(BlueprintCallable, Category = "Game|Weapon")
		class ANexosWeaponBase* GetLastWeapon() const;

	/** Global notification that a player has drawn a weapon */
	static FOnNexosPlayerDrawWeapon NotifyWeaponDrawn;

	/** Global notification that a player has stowed a weapon */
	static FOnNexosPlayerStowWeapon NotifyWeaponStowed;

	/** get weapon attach point */
	FName GetWeaponAttachPoint() const;

	FName GetStowedWeaponAttachPoint() const;

	/**
	 *get total number of weapons equipped
	 *this should not be needed, but may have use later on
	 */
	int32 GetInventoryCount() const;

	
	/**
	 * get weapon from inventory index. Index validity is not checked.
	 */
	class ANexosWeaponBase* GetInventoryWeapon(int32 Index) const;

	/* get weapon aiming modifier speed */
	UFUNCTION(BlueprintCallable, Category = "Game|Weapon")
		float GetAimingSpeedModifier() const;

	UFUNCTION(BlueprintCallable, Category = "Game|Weapon")
		bool IsAiming() const;

	UFUNCTION(BlueprintCallable, Category = "Game|Weapon")
		bool IsFiring() const;

	UFUNCTION(BlueprintCallable, Category = "Game|Abilities")
		bool IsThrowingGrenade() const;

	UFUNCTION(BlueprintCallable, Category = "Game|Abilities")
		bool IsInMelee() const;

	UFUNCTION(BlueprintCallable, Category = "Game|Abilities")
		bool IsUsingTactical() const;

	UFUNCTION(BlueprintCallable, Category = "Game|Abilities")
		bool IsCastingSingularity() const;

	/** get the modifier value for running speed */
	UFUNCTION(BlueprintCallable, Category = Pawn)
        float GetRunSpeedModifier() const;

	/** get running state */
	UFUNCTION(BlueprintCallable, Category = Pawn)
        bool IsRunning() const;

	UFUNCTION(BlueprintCallable, Category = Pawn)
		float GetJumpHeightModifier() const;

	UFUNCTION(BlueprintCallable, Category = Pawn)
		bool IsJumping();
	
	/** get camera view type, should be switch off for Social Spaces and Melee or 'special' weapons */
	UFUNCTION(BlueprintCallable, Category = Mesh)
        virtual bool IsFirstPerson() const;

	/** check if pawn is still alive */
	bool IsAlive() const;

	/*
	* Get either first or third person mesh.
	*
	* @param	WantFirstPerson		If true returns the first peron mesh, else returns the third
	*/
	USkeletalMeshComponent* GetSpecifcPawnMesh(bool WantFirstPerson) const;

	/** Take damage, handle death */
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) override;

	/** Pawn suicide */
	virtual void Suicide();

	/** Kill this pawn */
	virtual void KilledBy(class APawn* EventInstigator);

	/** Returns True if the pawn can die in the current state */
	virtual bool CanDie(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser) const;

	/**
	* Kills pawn.  Server/authority only.
	* @param KillingDamage - Damage amount of the killing blow
	* @param DamageEvent - Damage event of the killing blow
	* @param Killer - Who killed this pawn
	* @param DamageCauser - the Actor that directly caused the damage (i.e. the Projectile that exploded, the Weapon that fired, etc)
	* @returns true if allowed
	*/
	virtual bool Die(float KillingDamage, struct FDamageEvent const& DamageEvent, class AController* Killer, class AActor* DamageCauser);

	// Die when we fall out of the world.
	virtual void FellOutOfWorld(const class UDamageType& dmgType) override;

	/** Called on the actor right before replication occurs */
	virtual void PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker) override;

private:

	/** The main controlling mesh for the 1pt person view Torso, this is to be only owner see/Owner no see */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* Mesh1P;

	/** The arms for the 1st person/Menu, only owner see/Owner no see */
	UPROPERTY(VisibleDefaultsOnly, Category = Armor)
	USkeletalMeshComponent* MeshHelmet1P;
	
	/** The arms for the 1st person/Menu, only owner see */
	UPROPERTY(VisibleDefaultsOnly, Category = Armor)
	USkeletalMeshComponent* MeshArms1P;

	/** The legs for the 1st person/Menu, only owner see */
	UPROPERTY(VisibleDefaultsOnly, Category = Armor)
	USkeletalMeshComponent* MeshLegs1P;
	
	/** The Ornament for the 1st person/Menu, only owner see/Owner no see */
	UPROPERTY(VisibleDefaultsOnly, Category = Armor)
	USkeletalMeshComponent* MeshClassOrnament1P;

	/** The arms for the 1st person, this is to be Owner no see */
	UPROPERTY(VisibleDefaultsOnly, Category = Armor)
	USkeletalMeshComponent* MeshHelmet3P;

	/** The arms for the 1st person, this is to be Owner no see */
	UPROPERTY(VisibleDefaultsOnly, Category = Armor)
	USkeletalMeshComponent* MeshArms3P;
	
	/** The legs for the 1st person, this is to be Owner no see */
	UPROPERTY(VisibleDefaultsOnly, Category = Armor)
	USkeletalMeshComponent* MeshLegs3P;

	/** The arms for the 1st person, this is to be Owner no see */
	UPROPERTY(VisibleDefaultsOnly, Category = Armor)
	USkeletalMeshComponent* MeshClassOrnament3P;

	/** Whether or not the character is moving (based on movement input). */
	bool IsMoving();
	

protected:

	/** socket or bone name for attaching weapon mesh */
	UPROPERTY(EditDefaultsOnly, Category = Inventory)
	FName WeaponAttachPoint;
	
	/** currently equipped weapon */
	UPROPERTY(Transient, ReplicatedUsing = OnRep_CurrentWeapon)
	class ANexosWeaponBase* CurrentWeapon;

	/** Replicate where this pawn was last hit and damaged */
	UPROPERTY(Transient, ReplicatedUsing = OnRep_LastTakeHitInfo)
	struct FTakeHitInfo LastTakeHitInfo;

	/** Time at which point the last take hit info for the actor times out and won't be replicated; Used to stop join-in-progress effects all over the screen */
	float LastTakeHitTimeTimeout;

	/** modifier for max movement speed */
	UPROPERTY(EditDefaultsOnly, Category = Inventory)
	float AimingSpeedModifier;

	/** current targeting state */
	UPROPERTY(Transient, Replicated)
	uint8 bIsAiming : 1;
	
	/** modifier for max movement speed */
	UPROPERTY(EditDefaultsOnly, Category = Pawn)
	float RunSpeedModifier;

	/** current running state */
	UPROPERTY(Transient, Replicated)
	uint8 bWantsToRun : 1;

	/** from gamepad running is toggled */
	uint8 bWantsToRunToggled : 1;

	/** current firing state */
	uint8 bWantsToFire : 1;

	/** when low shields effects should start */
	float LowSheildsPercentage;

	/** when low health effects should start */
	float LowHealthPercentage;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	float BaseTurnRate;

	/** Base lookup rate, in deg/sec. Other scaling may affect final lookup rate. */
	float BaseLookUpRate;

	/** sound played on death, local player only */
	UPROPERTY(EditDefaultsOnly, Category = Pawn)
	USoundCue* DeathSound;

	/** effect played on respawn */
	UPROPERTY(EditDefaultsOnly, Category = Pawn)
	UParticleSystem* RespawnFX;

	/** sound played on respawn */
	UPROPERTY(EditDefaultsOnly, Category = Pawn)
	USoundCue* RespawnSound;

	/** sound played when Sheilds is low */
	UPROPERTY(EditDefaultsOnly, Category = Pawn)
	USoundCue* LowShieldsSound;

	/** sound played when health is low */
	UPROPERTY(EditDefaultsOnly, Category = Pawn)
	USoundCue* LowHealthSound;

	/** sound played when running */
	UPROPERTY(EditDefaultsOnly, Category = Pawn)
	USoundCue* RunLoopSound;

	/** sound played when stop running */
	UPROPERTY(EditDefaultsOnly, Category = Pawn)
	USoundCue* RunStopSound;

	/** sound played when targeting state changes */
	UPROPERTY(EditDefaultsOnly, Category = Pawn)
	USoundCue* AimingSound;

	/** used to manipulate with run loop sound */
	UPROPERTY()
	UAudioComponent* RunLoopAC;

	/** hook to looped low shields sound used to stop/adjust volume */
	UPROPERTY()
	UAudioComponent* LowShieldsWarningPlayer;

	/** hook to looped low health sound used to stop/adjust volume */
	UPROPERTY()
	UAudioComponent* LowHealthWarningPlayer;

	/** handles sounds for running */
	void UpdateRunSounds();

	/** handle mesh visibility and updates */
	void UpdatePawnMeshes();

	/** Responsible for cleaning up bodies on clients. */
	virtual void TornOff();

	/** notification when killed, for both the server and client. */
	virtual void OnDeath(float KillingDamage, struct FDamageEvent const& DamageEvent, class APawn* InstigatingPawn, class AActor* DamageCauser);

	/** play effects on hit */
	virtual void PlayHit(float DamageTaken, struct FDamageEvent const& DamageEvent, class APawn* PawnInstigator, class AActor* DamageCauser);

	/** switch to ragdoll */
	void SetRagdollPhysics();

	/** sets up the replication for taking a hit */
	void ReplicateHit(float Damage, struct FDamageEvent const& DamageEvent, class APawn* InstigatingPawn, class AActor* DamageCauser, bool bKilled);

	/** play hit or death on client */
	UFUNCTION()
        void OnRep_LastTakeHitInfo();

	//////////////////////////////////////////////////////////////////////////
	// Inventory

	/** updates current weapon */
	void SetCurrentWeapon(class ANexosWeaponBase* NewWeapon, class ANexosWeaponBase* LastWeapon = NULL);

	/** current weapon rep handler */
	UFUNCTION()
        void OnRep_CurrentWeapon(class ANexosWeaponBase* LastWeapon);

	void GetInventoryFromController();
	
	/** [server] spawns default inventory */
	void UpdateInventory();

	/** [server] remove all weapons from inventory and destroy them */
	void DestroyInventory();

	/** equip weapon */
	UFUNCTION(reliable, server, WithValidation)
        void ServerEquipWeapon(class ANexosWeaponBase* NewWeapon);

	/** update targeting state */
	UFUNCTION(reliable, server, WithValidation)
        void ServerSetTargeting(bool bNewTargeting);

	/** update targeting state */
	UFUNCTION(reliable, server, WithValidation)
        void ServerSetRunning(bool bNewRunning, bool bToggle);

	/** Builds list of points to check for pausing replication for a connection*/
	void BuildPauseReplicationCheckPoints(TArray<FVector>& RelevancyCheckPoints);
};

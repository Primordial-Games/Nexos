// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine.h"
#include "ParticleDefinitions.h"
#include "SoundDefinitions.h"
#include "Net/UnrealNetwork.h"
#include "Online/NexosColosseumGameModeBase.h"
#include "Online/NexosColosseumGameState.h"
#include "Player/NexosPlayerCharacter.h"
#include "Player/NexosPlayerCharacterMovement.h"
#include "Player/NexosPlayerController.h"
#include "NexosClasses.h"


UENUM(Blueprintable)
enum class ENexosAbilityInputID : uint8
{
	None,
	Confirm,
	Cancel,
	Melee,
	Grenade,
	Tactical,
	Singularity,
	Sprint,
	MomentumDrive,
	Jump
};

UENUM(Blueprintable)
enum class ENexosRarityType : uint8
{
	Common,
	Uncommon,
	Rare,
	Legendary,
	Mythic
};

UENUM(Blueprintable)
enum class ENexosElementType : uint8
{
	Kinetic,
	Neutron,
	Electron,
	Graviton
};

UENUM(Blueprintable)
enum class ENexosWeaponSlots : uint8
{
	Primary,
	Secondary,
	Heavy,
	Sidearm
};

UENUM(Blueprintable)
enum class ENexosAmmoType : uint8
{
	Primary,
	Special,
	Heavy
};

UENUM(Blueprintable)
enum class ENexosWeaponType : uint8
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

UENUM(Blueprintable)
enum class ENexosArmorSlot : uint8
{
	Helmet,
	Arms,
	Torso,
	Legs,
	Ornament,
	NeuralLink,
};

UENUM(Blueprintable)
enum class ECusrsoType : uint8
{
	Pointer,
	PointerHighlight,
	Controller,
	ControllerHighlight,
};

class FNexosGameModuleImpl : public IModuleInterface
{
public:
	// IModuleInterface Methods
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	virtual bool IsGameModule() const override
	{
		return true;
	}
#if UE_SERVER
private:
	// Methods added to support GSDK
	void ConnectToPlayFabAgent();
	void LogInfo(FString message);
	void LogError(FString message);
#endif
};

class UBehaviorTreeComponent;
/** when you modify this, please note that this information can be saved with instances
* also DefaultEngine.ini [/Script/Engine.CollisionProfile] should match with this list **/
#define COLLISION_WEAPON		ECC_GameTraceChannel1
#define COLLISION_PROJECTILE	ECC_GameTraceChannel2
#define COLLISION_PICKUP		ECC_GameTraceChannel3

#define MAX_PLAYER_NAME_LENGTH 16


#ifndef NEXOS_CONSOLE_UI
/** Set to 1 to pretend we're building for console even on a PC, for testing purposes */
#define NEXOS_SIMULATE_CONSOLE_UI	0

#if PLATFORM_PS4 || PLATFORM_SWITCH || NEXOS_SIMULATE_CONSOLE_UI
#define NEXOS_CONSOLE_UI 1
#else
#define NEXOS_CONSOLE_UI 0
#endif
#endif

#ifndef NEXOS_XBOX_STRINGS
#define NEXOS_XBOX_STRINGS 0
#endif
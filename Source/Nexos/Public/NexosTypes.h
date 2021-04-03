// Fill out your copyright notice in the Description page of Project Settings.

#include "CoreMinimal.h"
#include "Materials/Material.h"
#include "UObject/WeakObjectPtrTemplates.h"
#include "UObject/Class.h"
#include "NexosTypes.generated.h"
#pragma once

namespace ENexosMatchState
{
	enum Type
	{
		Warmup,
		Playing,
		Won,
		Lost,
	};
}

namespace ENexosCrosshairDirection
{
	enum Type
	{
		Left = 0,
		Right = 1,
		Top = 2,
		Bottom = 3,
		Center = 4
	};
}

namespace ENexosHudPosition
{
	enum Type
	{
		Left = 0,
		FrontLeft = 1,
		Front = 2,
		FrontRight = 3,
		Right = 4,
		BackRight = 5,
		Back = 6,
		BackLeft = 7,
	};
}

/** keep in sync with ShooterImpactEffect */
UENUM()
namespace ENexosPhysMaterialType
{
	enum Type
	{
		Unknown,
		Dirt,
		Rock,
		Sand,
		Water,
		Glass,
		Cloth,
		Metal,
		Wood,
		Grass,
		Carpet,
		Concrete,
		Snow,
		Tile,
		Asphalt,
		Brick,
		Cardboard,
		Clay,
		Plastic,
		Electronics,
		Sheetrock,
		Plaster,
		Leaves,
		Blood,
		DemoLaser,
		DemoPlasma,
		DemoExplosion,
	};
}

namespace ENexosDialogType
{
	enum Type
	{
		None,
		Generic,
		ControllerDisconnected
	};
}

#define NEXOS_SURFACE_Default		SurfaceType_Default
#define NEXOS_SURFACE_Dirt  		SurfaceType1
#define NEXOS_SURFACE_Rock			SurfaceType2
#define NEXOS_SURFACE_Sand			SurfaceType3
#define NEXOS_SURFACE_Water			SurfaceType4
#define NEXOS_SURFACE_Glass			SurfaceType5
#define NEXOS_SURFACE_Cloth			SurfaceType6
#define NEXOS_SURFACE_Metal			SurfaceType7
#define NEXOS_SURFACE_Wood			SurfaceType8
#define NEXOS_SURFACE_Grass			SurfaceType9
#define NEXOS_SURFACE_Carpet		SurfaceType10
#define NEXOS_SURFACE_Concrete		SurfaceType11
#define NEXOS_SURFACE_Snow			SurfaceType12
#define NEXOS_SURFACE_Tile			SurfaceType13
#define NEXOS_SURFACE_Asphalt		SurfaceType14
#define NEXOS_SURFACE_Brick			SurfaceType15
#define NEXOS_SURFACE_Cardboard		SurfaceType16
#define NEXOS_SURFACE_Clay			SurfaceType17
#define NEXOS_SURFACE_Plastic		SurfaceType18
#define NEXOS_SURFACE_Electronics	SurfaceType19
#define NEXOS_SURFACE_Sheetrock		SurfaceType20
#define NEXOS_SURFACE_Plaster		SurfaceType21
#define NEXOS_SURFACE_Leaves		SurfaceType22
#define NEXOS_SURFACE_Blood			SurfaceType23
#define NEXOS_SURFACE_DemoLaser		SurfaceType24
#define NEXOS_SURFACE_DemoPlasma	SurfaceType25
#define NEXOS_SURFACE_DemoExplotion	SurfaceType26

USTRUCT()
struct FDecalData
{
	GENERATED_USTRUCT_BODY()

		/** material */
	UPROPERTY(EditDefaultsOnly, Category = Decal)
		UMaterial* DecalMaterial;

	/** quad size (width & height) */
	UPROPERTY(EditDefaultsOnly, Category = Decal)
		float DecalSize;

	/** lifespan */
	UPROPERTY(EditDefaultsOnly, Category = Decal)
		float LifeSpan;

	/** defaults */
	FDecalData()
		: DecalMaterial(nullptr)
		, DecalSize(256.f)
		, LifeSpan(10.f)
	{
	}
};

/** replicated information on a hit we've taken */
USTRUCT()
struct FTakeHitInfo
{
	GENERATED_USTRUCT_BODY()

		/** The amount of damage actually applied */
	UPROPERTY()
		float ActualDamage;

	/** The damage type we were hit with. */
	UPROPERTY()
		UClass* DamageTypeClass;

	/** Who hit us */
	UPROPERTY()
		TWeakObjectPtr<class ANexosPlayerCharacter> PawnInstigator;

	/** Who actually caused the damage */
	UPROPERTY()
		TWeakObjectPtr<class AActor> DamageCauser;

	/** Specifies which DamageEvent below describes the damage received. */
	UPROPERTY()
		int32 DamageEventClassID;

	/** Rather this was a kill */
	UPROPERTY()
		uint32 bKilled : 1;

private:

	/** A rolling counter used to ensure the struct is dirty and will replicate. */
	UPROPERTY()
		uint8 EnsureReplicationByte;

	/** Describes general damage. */
	UPROPERTY()
		FDamageEvent GeneralDamageEvent;

	/** Describes point damage, if that is what was received. */
	UPROPERTY()
		FPointDamageEvent PointDamageEvent;

	/** Describes radial damage, if that is what was received. */
	UPROPERTY()
		FRadialDamageEvent RadialDamageEvent;

public:
	FTakeHitInfo();

	FDamageEvent& GetDamageEvent();
	void SetDamageEvent(const FDamageEvent& DamageEvent);
	void EnsureReplication();
};
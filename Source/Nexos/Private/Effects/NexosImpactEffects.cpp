// Fill out your copyright notice in the Description page of Project Settings.

#include "Effects/NexosImpactEffects.h"
#include "Nexos.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicalMaterials/PhysicalMaterial.h"


ANexosImpactEffects::ANexosImpactEffects(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	SetAutoDestroyWhenFinished(true);
}


UParticleSystem* ANexosImpactEffects::GetImpactFX(TEnumAsByte<EPhysicalSurface> SurfaceType) const
{
	UParticleSystem* ImpactFX = NULL;

	switch (SurfaceType)
	{
	case NEXOS_SURFACE_Dirt:  			ImpactFX = DirtFX; break;
	case NEXOS_SURFACE_Rock:			ImpactFX = RockFX; break;
	case NEXOS_SURFACE_Sand:			ImpactFX = SandFX; break;
	case NEXOS_SURFACE_Water:			ImpactFX = WaterFX; break;
	case NEXOS_SURFACE_Glass:			ImpactFX = GlassFX; break;
	case NEXOS_SURFACE_Cloth:			ImpactFX = ClothFX; break;
	case NEXOS_SURFACE_Metal:			ImpactFX = MetalFX; break;
	case NEXOS_SURFACE_Wood:			ImpactFX = WoodFX; break;
	case NEXOS_SURFACE_Grass:			ImpactFX = GrassFX; break;
	case NEXOS_SURFACE_Carpet:			ImpactFX = CarpetFX; break;
	case NEXOS_SURFACE_Concrete:		ImpactFX = ConcreteFX; break;
	case NEXOS_SURFACE_Snow:			ImpactFX = SnowFX; break;
	case NEXOS_SURFACE_Tile:			ImpactFX = TileFX; break;
	case NEXOS_SURFACE_Asphalt:			ImpactFX = AsphaltFX; break;
	case NEXOS_SURFACE_Brick:			ImpactFX = BrickFX; break;
	case NEXOS_SURFACE_Cardboard:		ImpactFX = CardboardFX; break;
	case NEXOS_SURFACE_Clay:			ImpactFX = ClayFX; break;
	case NEXOS_SURFACE_Plastic:			ImpactFX = PlasticFX; break;
	case NEXOS_SURFACE_Electronics:		ImpactFX = ElectronicsFX; break;
	case NEXOS_SURFACE_Sheetrock:		ImpactFX = SheetrockFX; break;
	case NEXOS_SURFACE_Plaster:			ImpactFX = PlasterFX; break;
	case NEXOS_SURFACE_Leaves:			ImpactFX = LeavesFX; break;
	case NEXOS_SURFACE_Blood:			ImpactFX = BloodFX; break;
	case NEXOS_SURFACE_DemoLaser:		ImpactFX = DemoLaserFX; break;
	case NEXOS_SURFACE_DemoPlasma:		ImpactFX = DemoPlasmaFX; break;
	case NEXOS_SURFACE_DemoExplotion:	ImpactFX = DemoExplosionFX; break;
	default:							ImpactFX = DefaultFX; break;
	}

	return ImpactFX;
}

USoundCue* ANexosImpactEffects::GetImpactSound(TEnumAsByte<EPhysicalSurface> SurfaceType) const
{
	USoundCue* ImpactSound = NULL;

	switch (SurfaceType)
	{
	case NEXOS_SURFACE_Concrete:	ImpactSound = ConcreteSound; break;
	case NEXOS_SURFACE_Dirt:		ImpactSound = DirtSound; break;
	case NEXOS_SURFACE_Water:		ImpactSound = WaterSound; break;
	case NEXOS_SURFACE_Metal:		ImpactSound = MetalSound; break;
	case NEXOS_SURFACE_Wood:		ImpactSound = WoodSound; break;
	case NEXOS_SURFACE_Grass:		ImpactSound = GrassSound; break;
	case NEXOS_SURFACE_Glass:		ImpactSound = GlassSound; break;
	case NEXOS_SURFACE_Blood:		ImpactSound = FleshSound; break;
	default:						ImpactSound = DefaultSound; break;
	}

	return ImpactSound;
}

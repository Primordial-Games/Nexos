// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NexosTypes.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Sound/SoundCue.h"
#include "NexosImpactEffects.generated.h"

UCLASS()
class NEXOS_API ANexosImpactEffects : public AActor
{
	GENERATED_UCLASS_BODY()

		/** default impact FX used when material specific override doesn't exist */
		UPROPERTY(EditDefaultsOnly, Category = Defaults)
		UParticleSystem* DefaultFX;

	/** impact FX on dirt */
	UPROPERTY(EditDefaultsOnly, Category = Visual)
		UParticleSystem* DirtFX;

	/** impact FX on rock */
	UPROPERTY(EditDefaultsOnly, Category = Visual)
		UParticleSystem* RockFX;

	/** impact FX on dirt */
	UPROPERTY(EditDefaultsOnly, Category = Visual)
		UParticleSystem* SandFX;

	/** impact FX on water */
	UPROPERTY(EditDefaultsOnly, Category = Visual)
		UParticleSystem* WaterFX;

	/** impact FX on glass */
	UPROPERTY(EditDefaultsOnly, Category = Visual)
		UParticleSystem* GlassFX;

	/** impact FX on glass */
	UPROPERTY(EditDefaultsOnly, Category = Visual)
		UParticleSystem* ClothFX;

	/** impact FX on metal */
	UPROPERTY(EditDefaultsOnly, Category = Visual)
		UParticleSystem* MetalFX;

	/** impact FX on wood */
	UPROPERTY(EditDefaultsOnly, Category = Visual)
		UParticleSystem* WoodFX;

	/** impact FX on grass */
	UPROPERTY(EditDefaultsOnly, Category = Visual)
		UParticleSystem* GrassFX;

	/** impact FX on grass */
	UPROPERTY(EditDefaultsOnly, Category = Visual)
		UParticleSystem* CarpetFX;

	/** impact FX on concrete */
	UPROPERTY(EditDefaultsOnly, Category = Visual)
		UParticleSystem* ConcreteFX;

	/** impact FX on grass */
	UPROPERTY(EditDefaultsOnly, Category = Visual)
		UParticleSystem* SnowFX;

	/** impact FX on grass */
	UPROPERTY(EditDefaultsOnly, Category = Visual)
		UParticleSystem* TileFX;

	/** impact FX on grass */
	UPROPERTY(EditDefaultsOnly, Category = Visual)
		UParticleSystem* AsphaltFX;

	/** impact FX on grass */
	UPROPERTY(EditDefaultsOnly, Category = Visual)
		UParticleSystem* BrickFX;

	/** impact FX on grass */
	UPROPERTY(EditDefaultsOnly, Category = Visual)
		UParticleSystem* CardboardFX;

	/** impact FX on grass */
	UPROPERTY(EditDefaultsOnly, Category = Visual)
		UParticleSystem* ClayFX;

	/** impact FX on grass */
	UPROPERTY(EditDefaultsOnly, Category = Visual)
		UParticleSystem* PlasticFX;

	/** impact FX on grass */
	UPROPERTY(EditDefaultsOnly, Category = Visual)
		UParticleSystem* ElectronicsFX;

	/** impact FX on grass */
	UPROPERTY(EditDefaultsOnly, Category = Visual)
		UParticleSystem* SheetrockFX;

	/** impact FX on grass */
	UPROPERTY(EditDefaultsOnly, Category = Visual)
		UParticleSystem* PlasterFX;

	/** impact FX on grass */
	UPROPERTY(EditDefaultsOnly, Category = Visual)
		UParticleSystem* LeavesFX;

	/** impact FX on flesh */
	UPROPERTY(EditDefaultsOnly, Category = Visual)
		UParticleSystem* BloodFX;

	/** impact FX on grass */
	UPROPERTY(EditDefaultsOnly, Category = Visual)
		UParticleSystem* DemoLaserFX;

	/** impact FX on grass */
	UPROPERTY(EditDefaultsOnly, Category = Visual)
		UParticleSystem* DemoPlasmaFX;

	/** impact FX on grass */
	UPROPERTY(EditDefaultsOnly, Category = Visual)
		UParticleSystem* DemoExplosionFX;

	/** default impact sound used when material specific override doesn't exist */
	UPROPERTY(EditDefaultsOnly, Category = Defaults)
		USoundCue* DefaultSound;

	/** impact FX on concrete */
	UPROPERTY(EditDefaultsOnly, Category = Sound)
		USoundCue* ConcreteSound;

	/** impact FX on dirt */
	UPROPERTY(EditDefaultsOnly, Category = Sound)
		USoundCue* DirtSound;

	/** impact FX on water */
	UPROPERTY(EditDefaultsOnly, Category = Sound)
		USoundCue* WaterSound;

	/** impact FX on metal */
	UPROPERTY(EditDefaultsOnly, Category = Sound)
		USoundCue* MetalSound;

	/** impact FX on wood */
	UPROPERTY(EditDefaultsOnly, Category = Sound)
		USoundCue* WoodSound;

	/** impact FX on glass */
	UPROPERTY(EditDefaultsOnly, Category = Sound)
		USoundCue* GlassSound;

	/** impact FX on grass */
	UPROPERTY(EditDefaultsOnly, Category = Sound)
		USoundCue* GrassSound;

	/** impact FX on flesh */
	UPROPERTY(EditDefaultsOnly, Category = Sound)
		USoundCue* FleshSound;

	/** default decal when material specific override doesn't exist */
	UPROPERTY(EditDefaultsOnly, Category = Defaults)
		struct FDecalData DefaultDecal;

	/** surface data for spawning */
	UPROPERTY(BlueprintReadOnly, Category = Surface)
		FHitResult SurfaceHit;



protected:

	/** get FX for material type */
	UParticleSystem* GetImpactFX(TEnumAsByte<EPhysicalSurface> SurfaceType) const;

	/** get sound for material type */
	USoundCue* GetImpactSound(TEnumAsByte<EPhysicalSurface> SurfaceType) const;
};

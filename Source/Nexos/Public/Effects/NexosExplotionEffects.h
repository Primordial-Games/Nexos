// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NexosTypes.h"
#include "CoreMinimal.h"
#include "Components/PointLightComponent.h"
#include "Sound/SoundCue.h"
#include "GameFramework/Actor.h"
#include "NexosExplotionEffects.generated.h"

UCLASS()
class NEXOS_API ANexosExplotionEffects : public AActor
{
	GENERATED_UCLASS_BODY()

		/** explosion FX */
		UPROPERTY(EditDefaultsOnly, Category = "Effect")
		UParticleSystem* ExplosionFX;

private:
	/** explosion light */
	UPROPERTY(VisibleDefaultsOnly, Category = "Effect")
		UPointLightComponent* ExplosionLight;
public:

	/** how long keep explosion light on? */
	UPROPERTY(EditDefaultsOnly, Category = "Effect")
		float ExplosionLightFadeOut;

	/** explosion sound */
	UPROPERTY(EditDefaultsOnly, Category = "Effect")
		USoundCue* ExplosionSound;

	/** explosion decals */
	UPROPERTY(EditDefaultsOnly, Category = "Effect")
		struct FDecalData Decal;

	/** surface data for spawning */
	UPROPERTY(BlueprintReadOnly, Category = "Surface")
		FHitResult SurfaceHit;

	/** update fading light */
	virtual void Tick(float DeltaSeconds) override;

protected:
	/** spawn explosion */
	virtual void BeginPlay() override;

private:

	/** Point light component name */
	FName ExplosionLightComponentName;

public:
	/** Returns ExplosionLight subobject **/
	FORCEINLINE UPointLightComponent* GetExplosionLight() const { return ExplosionLight; }

};

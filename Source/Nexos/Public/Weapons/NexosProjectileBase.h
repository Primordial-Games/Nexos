// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Weapons/NexosWeapon_Projectile.h"
#include "NexosProjectileBase.generated.h"

class UProjectileMovementComponent;
class USphereComponent;

// 
UCLASS(Abstract, Blueprintable)
class ANexosProjectileBase : public AActor
{
	GENERATED_UCLASS_BODY()

		/** initial setup */
		virtual void PostInitializeComponents() override;

	/** setup velocity */
	void InitVelocity(FVector& ShootDirection);

	/** handle hit */
	UFUNCTION()
		void OnImpact(const FHitResult& HitResult);

private:
	/** movement component */
	UPROPERTY(VisibleDefaultsOnly, Category = "Projectile")
		UProjectileMovementComponent* MovementComp;

	/** collisions */
	UPROPERTY(VisibleDefaultsOnly, Category = "Projectile")
		USphereComponent* CollisionComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Projectile")
		UParticleSystemComponent* ParticleComp;
protected:

	/** effects for explosion */
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
		TSubclassOf<class ANexosExplotionEffects> ExplosionTemplate;

	/** controller that fired me (cache for damage calculations) */
	TWeakObjectPtr<AController> MyController;

	/** projectile data */
	struct FProjectileWeaponData WeaponConfig;

	/** did it explode? */
	UPROPERTY(Transient, ReplicatedUsing = OnRep_Exploded)
		bool bExploded;

	/** [client] explosion happened */
	UFUNCTION()
		void OnRep_Exploded();

	/** trigger explosion */
	void Explode(const FHitResult& Impact);

	/** shutdown projectile and prepare for destruction */
	void DisableAndDestroy();

	/** update velocity on client */
	virtual void PostNetReceiveVelocity(const FVector& NewVelocity) override;

protected:
	/** Returns MovementComp subobject **/
	FORCEINLINE UProjectileMovementComponent* GetMovementComp() const { return MovementComp; }
	/** Returns CollisionComp subobject **/
	FORCEINLINE USphereComponent* GetCollisionComp() const { return CollisionComp; }
	/** Returns ParticleComp subobject **/
	FORCEINLINE UParticleSystemComponent* GetParticleComp() const { return ParticleComp; }
};


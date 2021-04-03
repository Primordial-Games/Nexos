// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "NexosPlayerCharacterBase.generated.h"

UCLASS(Config=Game)
class NEXOS_API ANexosPlayerCharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "Abilities", meta = (AllowPrivateAccess = "true"))
		class UAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY()
		class UNexosAttributeSet* PlayerAttributes;

	virtual void InitializeAttributes();
	virtual void GiveAbilities();

	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

	UPROPERTY(EditAnywhere, Category = "Abilities")
		TSubclassOf<class UGameplayEffect> DefaultAttributeEffects;
	
	// Sets default values for this character's properties
	ANexosPlayerCharacterBase();

	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = Abilities)
		TArray<TSubclassOf<class UNexosGameplayAbility>> DefaultAbilities;


protected:
	
	/* Player Weapon Loadout
	 * This is copied from the player controller whenever a character is spawned
	 **/
	UPROPERTY(EditDefaultsOnly, Category = Inventory)
		TMap<FString, TSubclassOf<class ANexosWeaponBase> > PlayerEquippedWeapons;

	/* Player Armor Loadout
	 * This is copied from the player controller whenever a character is spawned
	**/
	UPROPERTY(EditDefaultsOnly, Category = Inventory)
		TMap<FString, TSubclassOf<class ANexosArmorBase> > PlayerEquippedArmor;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};

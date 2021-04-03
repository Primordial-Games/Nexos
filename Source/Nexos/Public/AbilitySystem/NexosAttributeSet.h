/*
*Copyright (c) 2021 Primordial Games LLC
* All rights reserved. 
*/

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "NexosAttributeSet.generated.h"


//Use macros from Attributes.h
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 *
 */
UCLASS()
class NEXOS_API UNexosAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:

	UNexosAttributeSet();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


	/**
	 *Health and Shields, handled by the ability system
	 */
	
	/*
	 *Health setters and getters
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Character Stats", ReplicatedUsing = OnRep_Health)
		FGameplayAttributeData Health;
	
	ATTRIBUTE_ACCESSORS(UNexosAttributeSet, Health);

	UFUNCTION()
        virtual void OnRep_Health(const FGameplayAttributeData& OldHealth);

	/*
	 *Shield setters and getters
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Character Stats", ReplicatedUsing = OnRep_Shield)
		FGameplayAttributeData Shield;
	ATTRIBUTE_ACCESSORS(UNexosAttributeSet, Shield);

	UFUNCTION()
		virtual void OnRep_Shield(const FGameplayAttributeData& OldShield);


	//////////////////////////////////////////////////////////////////////////////////
	/**
	 *CharacterStats, these are affected by Armor pieces, Armor Attachments/Mods, and class builds
	 */
	//////////////////////////////////////////////////////////////////////////////////
	
	//Character Combat Rating, the combined Attack and Defence of all equip weapons and armor.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Stats")
	FGameplayAttribute CombatRating;
	
	//Character Constitution, used to calculate Max Health and Shields
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Stats")
	FGameplayAttribute Constitution;

	//Character Restoration, used to calculate Health and Shields Regen Rates
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Stats")
	FGameplayAttribute Restoration;

	//Character Dexterity, used to calculate movement speed and jump height
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Stats")
	FGameplayAttribute Dexterity;

	//Character Strength, used to calculate Melee attacks damage and recharge rate
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Stats")
	FGameplayAttribute Strength;

	//Character Intelligence, used to calculate Grenade and Tactical abilities damage and recharge rate
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Stats")
	FGameplayAttribute Intelligence;

	//Character Wisdom, used to calculate Singularity Duration, damage, and recharge rate
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Stats")
	FGameplayAttribute Wisdom;

	//////////////////////////////////////////////////////////////////////////////////
	/**
	*Ability Energy, This is to determine if a character can use an ability at any given moment
	*/
	//////////////////////////////////////////////////////////////////////////////////

	//Melee Energy, if full use Melee ability otherwise use default melee attack
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability Energy")
	FGameplayAttribute MeleeEnergy;

	//Melee Charges, How many times can a player use a melee ability back to back
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability Energy")
	FGameplayAttribute MeleeCharges;
	
	//Grenade Energy, if full player can throw a grenade
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability Energy")
	FGameplayAttribute GrenadeEnergy;

	//Grenade Charges, How many times can a player use a Grenade ability back to back
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability Energy")
	FGameplayAttribute GrenadeCharges;
	
	//Tactical Energy, if full player can use their Tactical Ability
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability Energy")
	FGameplayAttribute TacticalEnergy;

	//Tactical Charges, How many times can a player use a Tactical ability back to back
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability Energy")
	FGameplayAttribute TacticalCharges;

	//Singularity Energy, if full player can use their Singularity Ability
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability Energy")
	FGameplayAttribute SingularityEnergy;
};

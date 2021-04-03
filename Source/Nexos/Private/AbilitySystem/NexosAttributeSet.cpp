// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/NexosAttributeSet.h"
#include "Net/UnrealNetwork.h"


UNexosAttributeSet::UNexosAttributeSet()
{
}

void UNexosAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UNexosAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UNexosAttributeSet, Shield, COND_None, REPNOTIFY_Always);
}

void UNexosAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UNexosAttributeSet, Health, OldHealth);
}

void UNexosAttributeSet::OnRep_Shield(const FGameplayAttributeData& OldShield)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UNexosAttributeSet, Shield, OldShield);
}

// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/NexosPlayerCharacterMovement.h"
#include "Nexos.h"
#include "Player/NexosPlayerCharacter.h"



UNexosPlayerCharacterMovement::UNexosPlayerCharacterMovement(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}


float UNexosPlayerCharacterMovement::GetMaxSpeed() const
{
	float MaxSpeed = Super::GetMaxSpeed();

	const ANexosPlayerCharacter* NexosCharacterOwner = Cast<ANexosPlayerCharacter>(PawnOwner);
	if (NexosCharacterOwner)
	{
		if (NexosCharacterOwner->IsTargeting())
		{
			MaxSpeed *= NexosCharacterOwner->GetTargetingSpeedModifier();
		}
		if (NexosCharacterOwner->IsRunning())
		{
			MaxSpeed *= NexosCharacterOwner->GetRunningSpeedModifier();
		}
	}

	return MaxSpeed;
}
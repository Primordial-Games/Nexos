// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/NexosPlayerCameraManager.h"
#include "Player/NexosPlayerCharacter.h"


ANexosPlayerCameraManager::ANexosPlayerCameraManager(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	NormalFOV = 90.0f;
	TargetingFOV = 60.0f;
	ViewPitchMin = -70.0f;
	ViewPitchMax = 85.0f;

	bAlwaysApplyModifiers = true;
}

void ANexosPlayerCameraManager::UpdateCamera(float DeltaTime)
{
	ANexosPlayerCharacter* MyPawn = PCOwner ? Cast<ANexosPlayerCharacter>(PCOwner->GetPawn()) : NULL;
	if (MyPawn && MyPawn->IsFirstPerson())
	{
		const float TargetFOV = MyPawn->IsTargeting() ? TargetingFOV : NormalFOV;
		DefaultFOV = FMath::FInterpTo(DefaultFOV, TargetFOV, DeltaTime, 20.0f);
	}

	Super::UpdateCamera(DeltaTime);

	if (MyPawn && MyPawn->IsFirstPerson())
	{
		MyPawn->OnCameraUpdate(GetCameraLocation(), GetCameraRotation());
	}
}
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Player/NexosPlayerCharacter.h"
#include "ShooterBot.generated.h"

UCLASS()
class AShooterBot : public ANexosPlayerCharacter
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(EditAnywhere, Category=Behavior)
	class UBehaviorTree* BotBehavior;

	virtual bool IsFirstPerson() const override;

	virtual void FaceRotation(FRotator NewRotation, float DeltaTime = 0.f) override;
};
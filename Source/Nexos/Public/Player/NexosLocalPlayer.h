// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LocalPlayer.h"
#include "NexosLocalPlayer.generated.h"

/**
 *
 */
UCLASS(config = Engine, transient)
class NEXOS_API UNexosLocalPlayer : public ULocalPlayer
{
	GENERATED_UCLASS_BODY()

public:

	virtual void SetControllerId(int32 NewControllerId) override;

	virtual FString GetNickname() const;

	class UNexosPersistentUser* GetPersistentUser() const;

	/** Initialize the PersistentUser */
	void LoadPersistentUser();

private:
	/** Persistent user data stored between sessions (i.e. the user's savegame) */
	UPROPERTY()
		class UNexosPersistentUser* PersistentUser;
};

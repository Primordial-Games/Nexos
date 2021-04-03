// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "UI/InviteWidget.h"
#include "Player/NexosPlayerControllerBase.h"
#include "NexosLobbyPlayerController.generated.h"

class FGameEvent;


USTRUCT(BlueprintType)
struct FFriendInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite)
	FString DisplayName;

	UPROPERTY(BlueprintReadWrite)
	FString FriendsEpicIdString;

	UPROPERTY(BlueprintReadWrite)
	uint8 status;

	UPROPERTY(BlueprintReadWrite)
	FString GameName;
	
};

/**
 * 
 */
UCLASS(Blueprintable)
class NEXOS_API ANexosLobbyPlayerController : public ANexosPlayerControllerBase
{
	GENERATED_UCLASS_BODY()
public:
		/** After game is initialized */
		virtual void PostInitializeComponents() override;

	UFUNCTION(BlueprintCallable, Category = "Online Services")
		void ServerTravelToMap(FString MapName);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player UI")
		TSubclassOf<class UInviteWidget> InviteWidgetClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player UI")
		class UInviteWidget* InviteWidgetInstance;

	UFUNCTION(BlueprintCallable, Category = "Online Services")
		virtual void PlayerRegionLatency();

};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WidgetController.generated.h"

UCLASS(Blueprintable)
class NEXOS_API AWidgetController : public AActor
{
	GENERATED_UCLASS_BODY()
	
public:	
	
	UFUNCTION(BlueprintNativeEvent)
		void ShowTitleLoadScreen();

	UFUNCTION(BlueprintNativeEvent)
		void ShowLoginMenu();

	UFUNCTION(BlueprintNativeEvent)
		void InvitePopup();

	UFUNCTION(BlueprintNativeEvent)
		void RemoveInvitePopup();

};

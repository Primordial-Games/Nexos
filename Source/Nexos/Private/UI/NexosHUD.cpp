// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/NexosHUD.h"

ANexosHUD::ANexosHUD(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void ANexosHUD::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
}

void ANexosHUD::DrawHUD()
{
}

void ANexosHUD::NotifyWeaponHit(float DamageTaken, FDamageEvent const& DamageEvent, APawn* PawnInstigator)
{
}

void ANexosHUD::NotifyOutOfAmmo()
{
}

void ANexosHUD::NotifyEnemyHit()
{
}

void ANexosHUD::SetMatchState(ENexosMatchState::Type NewState)
{
}

ENexosMatchState::Type ANexosHUD::GetMatchState() const
{
	return ENexosMatchState::Type();
}

void ANexosHUD::ConditionalCloseScoreboard(bool bFocus)
{
}

void ANexosHUD::ToggleScoreboard()
{
}

bool ANexosHUD::ShowScoreboard(bool bEnable, bool bFocus)
{
	return false;
}

void ANexosHUD::ShowDeathMessage(ANexosPlayerState* KillerPlayerState, ANexosPlayerState* VictimPlayerState, const UDamageType* KillerDamageType)
{
}

void ANexosHUD::ToggleChat()
{
}

void ANexosHUD::SetChatVisibilty(const EVisibility RequiredVisibility)
{
}

void ANexosHUD::AddChatLine(const FText& ChatString, bool bWantFocus)
{
}

bool ANexosHUD::IsMatchOver() const
{
	return false;
}

void ANexosHUD::PostInitializeComponents()
{
}

FString ANexosHUD::GetTimeString(float TimeSeconds)
{
	return FString();
}

void ANexosHUD::DrawWeaponHUD()
{
}

void ANexosHUD::DrawKills()
{
}

void ANexosHUD::DrawHealth()
{
}

void ANexosHUD::DrawShields()
{
}

void ANexosHUD::DrawMatchTimerAndPosition()
{
}

void ANexosHUD::DrawCrosshair()
{
}

void ANexosHUD::DrawHitIndicator()
{
}

void ANexosHUD::DrawDeathMessages()
{
}

float ANexosHUD::DrawRecentlyKilledPlayer()
{
	return 0.0f;
}

void ANexosHUD::DrawDebugInfoString(const FString& Text, float PosX, float PosY, bool bAlignLeft, bool bAlignTop, const FColor& TextColor)
{
}

void ANexosHUD::MakeUV(FCanvasIcon& Icon, FVector2D& UV0, FVector2D& UV1, uint16 U, uint16 V, uint16 UL, uint16 VL)
{
}

bool ANexosHUD::TryCreateChatWidget()
{
	return false;
}

void ANexosHUD::AddMatchInfoString(const FCanvasTextItem InfoItem)
{
}

float ANexosHUD::ShowInfoItems(float YOffset, float TextScale)
{
	return 0.0f;
}

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"



#include "PlayFabJsonObject.h"
#include "GameFramework/SaveGame.h"
#include "NexosPersistentUser.generated.h"

/**
 *
 */
UCLASS()
class UNexosPersistentUser : public USaveGame
{
	GENERATED_UCLASS_BODY()

public:
	/** Load user persistence data if it exists, create an empty record otherwise */
	static UNexosPersistentUser* LoadPersistentUser(FString SlotName, const int32 UserIndex);

	/** saves data if anything has changed */
	void SaveIfDirty();

	/** Record the results of a match */
	void AddMatchResults(int32 MatchKills, int32 MatchDeaths, int32 MatchBulletsFired, bool bIsMatchWinner);

	/** needed because we can recreate the subsystem that stores it */
	void TellInputAboutKeybindings();

	int32 GetUserIndex() const;

	UPROPERTY(BlueprintReadWrite)
		FString PlayerName;

	UPROPERTY(BlueprintReadWrite)
		FString PlatformId;

	UPROPERTY(BlueprintReadWrite)
		FString PlayFabId;

	UPROPERTY(BlueprintReadWrite)
		UPlayFabJsonObject* EntityKey;

	UPROPERTY(BlueprintReadWrite)
		UPlayFabJsonObject* Atributes;
	
	FORCEINLINE int32 GetKills() const
	{
		return Kills;
	}

	FORCEINLINE int32 GetDeaths() const
	{
		return Deaths;
	}

	FORCEINLINE int32 GetWins() const
	{
		return Wins;
	}

	FORCEINLINE int32 GetLosses() const
	{
		return Losses;
	}

	FORCEINLINE int32 GetBulletsFired() const
	{
		return BulletsFired;
	}

	/** Is controller vibration turned on? */
	FORCEINLINE bool GetVibration() const
	{
		return bVibrationOpt;
	}

	/** Is the y axis inverted? */
	FORCEINLINE bool GetInvertedYAxis() const
	{
		return bInvertedYAxis;
	}

	/** Setter for controller vibration option */
	void SetVibration(bool bVibration);

	/** Setter for inverted y axis */
	void SetInvertedYAxis(bool bInvert);

	/** Getter for the aim sensitivity */
	FORCEINLINE float GetAimSensitivity() const
	{
		return AimSensitivity;
	}

	void SetAimSensitivity(float InSensitivity);

	/** Getter for the gamma correction */
	FORCEINLINE float GetGamma() const
	{
		return Gamma;
	}

	void SetGamma(float InGamma);


	FORCEINLINE bool IsRecordingDemos() const
	{
		return bIsRecordingDemos;
	}

	void SetIsRecordingDemos(const bool InbIsRecordingDemos);

	FORCEINLINE FString GetName() const
	{
		return SlotName;
	}

protected:
	void SetToDefaults();

	/** Checks if the Mouse Sensitivity user setting is different from current */
	bool IsAimSensitivityDirty() const;

	/** Checks if the Inverted Mouse user setting is different from current */
	bool IsInvertedYAxisDirty() const;

	/** Triggers a save of this data. */
	void SavePersistentUser();

	/** Lifetime count of kills */
	UPROPERTY()
		int32 Kills;

	/** Lifetime count of deaths */
	UPROPERTY()
		int32 Deaths;

	/** Lifetime count of match wins */
	UPROPERTY()
		int32 Wins;

	/** Lifetime count of match losses */
	UPROPERTY()
		int32 Losses;

	/** Lifetime count of bullets fired */
	UPROPERTY()
		int32 BulletsFired;


	/** is recording demos? */
	UPROPERTY()
		bool bIsRecordingDemos;

	/** Holds the gamma correction setting */
	UPROPERTY()
		float Gamma;

	/** Holds the mouse sensitivity */
	UPROPERTY()
		float AimSensitivity;

	/** Is the y axis inverted or not? */
	UPROPERTY()
		bool bInvertedYAxis;

	UPROPERTY()
		bool bVibrationOpt;

private:
	/** Internal.  True if data is changed but hasn't been saved. */
	bool bIsDirty;

	/** The string identifier used to save/load this persistent user. */
	FString SlotName;
	int32 UserIndex;

};

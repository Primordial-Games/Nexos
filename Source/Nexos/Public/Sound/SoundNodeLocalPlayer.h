// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sound/SoundNode.h"
#include "SoundNodeLocalPlayer.generated.h"

/**
 *
 */
UCLASS(hidecategories = Object, editinlinenew)
class NEXOS_API USoundNodeLocalPlayer : public USoundNode
{
	GENERATED_UCLASS_BODY()

		virtual void ParseNodes(FAudioDevice* AudioDivice, const UPTRINT NodeWaveInstanceHash, FActiveSound& ActiveSound, const FSoundParseParameters& ParseParams, TArray<FWaveInstance*>& WaveInstance) override;
	virtual int32 GetMaxChildNodes() const override;
	virtual int32 GetMinChildNodes() const override;
#if WITH_EDITOR
	virtual FText GetInputPinName(int32 PinIndex) const override;
#endif


	static TMap<uint32, bool>& GetLocallyControlledActorCache()
	{
		check(IsInAudioThread());
		return LocallyControlledActorCache;
	}

private:

	static TMap<uint32, bool> LocallyControlledActorCache;


};

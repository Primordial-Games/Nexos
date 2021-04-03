// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NexosArmorBase.generated.h"

UCLASS()
class NEXOS_API ANexosArmorBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANexosArmorBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleDefaultsOnly, Category = Config)
	FString ArmorName;

	UPROPERTY(VisibleDefaultsOnly, Category = Config)
	FString InstanceId;

	UPROPERTY(VisibleDefaultsOnly, Category = Stats)
	int32 Dexterity;

	UPROPERTY(VisibleDefaultsOnly, Category = Stats)
	int32 Constitution;

	UPROPERTY(VisibleDefaultsOnly, Category = Stats)
	int32 Restoration;

	UPROPERTY(VisibleDefaultsOnly, Category = Stats)
	int32 Wisdom;

	UPROPERTY(VisibleDefaultsOnly, Category = Stats)
	int32 Intelligence;

	UPROPERTY(VisibleDefaultsOnly, Category = Stats)
	int32 Strength;

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMesh* ArmorMesh;

protected:
	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE USkeletalMesh* GetArmorMesh() const { return ArmorMesh; }

};

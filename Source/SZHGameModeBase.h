// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "EngineMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Engine/DataTable.h"
#include "ZombieSpawner.h"
#include "SZHGameModeBase.generated.h"

/**
*
*/
UCLASS()
class SZH_API ASZHGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
private:
	int currentStage;
	const int maxStage = 10;
	int currentResource;
	const int maxResource = 5000;

	UPROPERTY()
	TArray<UDataTable*> orderTable;
	TQueue<FOrderTableRow*>* orderQueue;

	float remainWaitTime;

	int remainZombie;

	AZombieSpawner* zombieSpawner_Left;
	AZombieSpawner* zombieSpawner_Right;

	float menuReturnTime;

	UPROPERTY()
	TArray<USoundBase*> roundCounts;
	bool* is_Counted;

private:
	void ReadStageInfo();

public:
	ASZHGameModeBase();

	virtual void PostInitializeComponents() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	int GetCurrentStage();

	UFUNCTION(BlueprintCallable)
	int GetCurrentResource();
	bool IncreaseResource(int value);

	void SetRemainZombies(int value);
	UFUNCTION(BlueprintCallable)
	int GetRemainZombies();

	UPROPERTY(BlueprintReadWrite)
	bool is_GameOver;
	UPROPERTY(BlueprintReadOnly)
	bool is_GameClear;

	void SetLeftZombieSpawner(AZombieSpawner* spawner);
	void SetRightZombieSpawner(AZombieSpawner* spawner);
};

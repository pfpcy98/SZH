// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.


#include "SZHGameModeBase.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "PlayerCharacter.h"
#include "PlayerCharacterController.h"
#include "UObject/ConstructorHelpers.h"


ASZHGameModeBase::ASZHGameModeBase()
{
	// Tick 함수 활성화
	PrimaryActorTick.bCanEverTick = true;

	// GameModeBase 값 할당
	DefaultPawnClass = APlayerCharacter::StaticClass();
	PlayerControllerClass = APlayerCharacterController::StaticClass();

	// 내부 변수 초기화
	currentStage = 1;
	currentResource = 50;

	remainZombie = 0;

	remainWaitTime = 0.0f;

	is_GameOver = false;
	is_GameClear = false;
}

void ASZHGameModeBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	ReadStageInfo();
}

void ASZHGameModeBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (remainWaitTime <= 0.0f)
	{
		if (!is_Counted[currentStage - 1])
		{
			UE_LOG(LogTemp, Warning, TEXT("%d"), currentStage - 1);
			is_Counted[currentStage - 1] = true;
			UGameplayStatics::PlaySound2D(GetWorld(), roundCounts[currentStage - 1]);
		}

		if (!orderQueue[currentStage - 1].IsEmpty())
		{
			FOrderTableRow* order;
			orderQueue[currentStage - 1].Dequeue(order);

			if (order->orderType == FString("wait"))
			{
				remainWaitTime += order->waitSecond;
			}
			else if (order->orderType == FString("spawn"))
			{
				if (order->spawnLine == FString("left"))
				{
					zombieSpawner_Left->AddQueue(order);
				}
				else if (order->spawnLine == FString("right"))
				{
					zombieSpawner_Right->AddQueue(order);
				}
			}
		}
	}
	else
	{
		remainWaitTime -= DeltaTime;
		if (remainWaitTime <= 0.0f)
		{
			remainWaitTime = 0.0f;
		}
	}

	if (orderQueue[currentStage - 1].IsEmpty() &&
		zombieSpawner_Left->IsQueueEmpty() &&
		zombieSpawner_Right->IsQueueEmpty() &&
		remainZombie <= 0)
	{
		if (currentStage + 1 <= maxStage)
		{
			remainWaitTime += 20.0f;
			currentStage++;
		}
		else
		{
			is_GameClear = true;
		}
	}
}

void ASZHGameModeBase::ReadStageInfo()
{
	// 라운드 플레이 정보 로딩
	orderQueue = new TQueue<FOrderTableRow*>[maxStage];

	for (int i = currentStage; i <= maxStage; i++)
	{
		FString path("/Game/StageInfo/stageInfo");
		path.Append(FString::FromInt(i));
		UE_LOG(LogTemp, Warning, TEXT("%s"), *path);
		
		UDataTable* dataTable = LoadObject<UDataTable>(NULL, *path, NULL, LOAD_None, NULL);

		if (::IsValid(dataTable))
		{
			UE_LOG(LogTemp, Warning, TEXT("Find!"));

			orderTable.Add(dataTable);
		}
	}

	for (int j = 0; j < maxStage; j++)
	{
		if (orderTable[j] != nullptr)
		{
			for (int i = 0; i < orderTable[j]->GetRowNames().Num(); i++)
			{
				FOrderTableRow* order = orderTable[j]->FindRow<FOrderTableRow>(orderTable[j]->GetRowNames()[i], FString(""));
				orderQueue[j].Enqueue(order);
			}
		}
	}

	// 라운드 호출음 로딩 및 변수 초기화
	is_Counted = new bool[maxStage];

	for (int i = currentStage; i <= maxStage; i++)
	{
		FString path("/Game/Sound/Effect/Rounds/RoundsAsset/");
		path.Append(FString::FromInt(i - 1));
		UE_LOG(LogTemp, Warning, TEXT("%s"), *path);

		USoundBase* sound = LoadObject<USoundBase>(NULL, *path, NULL, LOAD_None, NULL);

		if (::IsValid(sound))
		{
			roundCounts.Add(sound);
			is_Counted[i - 1] = false;
		}
	}
}

int ASZHGameModeBase::GetCurrentStage()
{
	return currentStage;
}

int ASZHGameModeBase::GetCurrentResource()
{
	return currentResource;
}

bool ASZHGameModeBase::IncreaseResource(int value)
{
	if (currentResource + value < 0)
	{
		return false;
	}

	if (currentResource + value > maxResource)
	{
		currentResource = maxResource;
	}
	else
	{
		currentResource = currentResource + value;
	}

	return true;
}

void ASZHGameModeBase::SetLeftZombieSpawner(AZombieSpawner* spawner)
{
	if (::IsValid(spawner))
	{
		zombieSpawner_Left = spawner;
	}
}

void ASZHGameModeBase::SetRightZombieSpawner(AZombieSpawner* spawner)
{
	if (::IsValid(spawner))
	{
		zombieSpawner_Right = spawner;
	}
}

void ASZHGameModeBase::SetRemainZombies(int value)
{
	remainZombie = value;
}

int ASZHGameModeBase::GetRemainZombies()
{
	return remainZombie;
}
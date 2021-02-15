// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "ZombieSpawner.generated.h"

USTRUCT(BlueprintType)
struct FOrderTableRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Order")
		FString orderType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Order")
		float waitSecond;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Order")
		FString spawnLine;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Order")
		FString zombieType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Order")
		int zombieQuantity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Order")
		int zombieHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Order")
		int zombieAttackPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Order")
		int zombieSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Order")
		float zombieAttackSpeed;
};

UCLASS()
class SZH_API AZombieSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	AZombieSpawner();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

private:
	UBoxComponent* rootBoxComponent;

	class ASZHGameModeBase* managerInstance;
	TQueue<FOrderTableRow*> zombieOrder;

	bool is_SomethingAbobe = false;
	FVector spawnPos;

public:
	UPROPERTY(EditAnywhere)
		bool is_Left;

private:
	UFUNCTION()
		void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:
	void AddQueue(FOrderTableRow* order);
	bool IsQueueEmpty();
};

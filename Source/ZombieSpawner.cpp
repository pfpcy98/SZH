// Fill out your copyright notice in the Description page of Project Settings.


#include "ZombieSpawner.h"
#include "SZHGameModeBase.h"
#include "ZombieHorde.h"

AZombieSpawner::AZombieSpawner()
{
 	PrimaryActorTick.bCanEverTick = true;

	rootBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("RootComponent"));
	RootComponent = rootBoxComponent;
	rootBoxComponent->SetBoxExtent(FVector(160.0f, 160.0f, 32.0f));
}

void AZombieSpawner::BeginPlay()
{
	Super::BeginPlay();
	
	rootBoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AZombieSpawner::OnOverlapBegin);
	rootBoxComponent->OnComponentEndOverlap.AddDynamic(this, &AZombieSpawner::OnOverlapEnd);

	AGameModeBase* mode = UGameplayStatics::GetGameMode(GetWorld());
	managerInstance = Cast<ASZHGameModeBase>(mode);

	if (is_Left)
	{
		managerInstance->SetLeftZombieSpawner(this);
	}
	else
	{
		managerInstance->SetRightZombieSpawner(this);
	}

	spawnPos = GetActorLocation();
	spawnPos.Z = spawnPos.Z + 160.0f;
}

void AZombieSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!is_SomethingAbobe &&
		!IsQueueEmpty())
	{
		FOrderTableRow* order;
		zombieOrder.Dequeue(order);

		AZombieHorde* zombie = (AZombieHorde*)GetWorld()->SpawnActor(AZombieHorde::StaticClass(), &spawnPos);
		if (zombie)
		{
			if (order->zombieType == FString("boss"))
			{
				zombie->is_Boss = true;
			}
			else
			{
				zombie->is_Boss = false;
			}
			zombie->zombieQuantity = order->zombieQuantity;
			zombie->zombieHealth = order->zombieHealth;
			zombie->zombieAttackPoint = order->zombieAttackPoint;
			zombie->zombieSpeed = order->zombieSpeed;
			zombie->zombieAttackSpeed = order->zombieAttackSpeed;
			UE_LOG(LogTemp, Warning, TEXT("Set"));
			zombie->LateInitialize();
		}
	}
}

void AZombieSpawner::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("Overlapped"));
		is_SomethingAbobe = true;
	}
}

void AZombieSpawner::OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	is_SomethingAbobe = false;
}

void AZombieSpawner::AddQueue(FOrderTableRow* order)
{
	zombieOrder.Enqueue(order);
}

bool AZombieSpawner::IsQueueEmpty()
{
	return zombieOrder.IsEmpty();
}
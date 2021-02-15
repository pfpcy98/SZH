// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "GameFramework/Pawn.h"
#include "Door.h"
#include "SZHGameModeBase.h"
#include "ZombieHorde.generated.h"

UENUM(BlueprintType)
enum class EZombieStatus : uint8
{
	E_IDLE UMETA(DisplayName = "Idle"),
	E_MOVE UMETA(DisplayName = "Move"),
	E_ATTACKREADY UMETA(DisplayName = "Attack Ready"),
	E_ATTACK UMETA(DisplayName = "Attack"),
	E_DEAD UMETA(DisplayName = "Dead"),
};

UCLASS()
class SZH_API AZombieHorde : public APawn
{
	GENERATED_BODY()

public:
	AZombieHorde();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

private:
	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void MoveFowardToTarget(float DeltaTime);
	void SightCheck();
	void StateChangeToIdle();

public:
	void LateInitialize();

private:
	UBoxComponent* rootBoxComponent;
	FVector zombiePos[4];
	USkeletalMeshComponent* zombieComponents[4];

	UPROPERTY()
	USkeletalMesh* normalZombieMesh[4];
	UPROPERTY()
	USkeletalMesh* bossZombieMesh;
	TSubclassOf<UAnimInstance> zombieAnimInstance;

	ASZHGameModeBase* managerInstance;

public:
	UAudioComponent* audioComponent;
	UPROPERTY()
	USoundBase* zombieAttackSound;
	UPROPERTY()
	USoundBase* zombieDeathSound;
	UPROPERTY()
	USoundBase* zombieIdleSound;

	UParticleSystem* zombieDeathParticle;

	EZombieStatus status = EZombieStatus::E_IDLE;

	bool is_Boss = false;

	int zombieQuantity = 1;
	int zombieHealth = 1;
	int zombieAttackPoint = 0;
	float zombieAttackSpeed = 1;
	int zombieSpeed = 1;
	int zombieScale = 1;
	int zombieDirection;
	int zombieSpace = 1;
	int zombieSoundPlay = 1;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool is_StuckByZombie = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool is_StuckByDoor = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FVector positionToMove = FVector(0.0f, 0.0f, 20.0f);

	bool is_Attacked;
	float attackCooldown;
	float idleSoundCooldown;

	ADoor* targetDoor;
};

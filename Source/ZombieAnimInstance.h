// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ZombieHorde.h"
#include "Animation/AnimInstance.h"
#include "ZombieAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class SZH_API UZombieAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
private:
	UFUNCTION()
	void AnimNotify_AttackComplete();

public:
	UZombieAnimInstance();

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	EZombieStatus status;
};

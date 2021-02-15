// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "GameFramework/Character.h"
#include "Components/BillboardComponent.h"
#include "PlayerCharacter.generated.h"

DECLARE_MULTICAST_DELEGATE(FInteractDelegate);

UCLASS()
class SZH_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	FVector inputDirection = FVector::ZeroVector;

public:
	UPROPERTY(VisibleAnywhere, Category = Camera)
		USpringArmComponent* springArm;

	UPROPERTY(VisibleAnywhere, Category = Camera)
		UCameraComponent* camera;

	UPROPERTY(VisibleAnywhere, Category = Camera)
		UBillboardComponent* billboard;

	FInteractDelegate InteractionWithStageObject;

	bool is_Carrying;

private:
	void MoveForward(float value);
	void MoveRight(float value);
	void Interact();
};

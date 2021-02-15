// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacterController.h"

void APlayerCharacterController::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	bShowMouseCursor = true;
	bEnableClickEvents = true;
}

void APlayerCharacterController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);
}
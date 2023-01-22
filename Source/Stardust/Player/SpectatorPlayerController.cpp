// Fill out your copyright notice in the Description page of Project Settings.


#include "SpectatorPlayerController.h"

ASpectatorPlayerController::ASpectatorPlayerController()
{
	bShowMouseCursor = true;
	bEnableMouseOverEvents = true;
	bEnableClickEvents = true;
}

void ASpectatorPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
}

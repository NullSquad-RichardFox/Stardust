// Fill out your copyright notice in the Description page of Project Settings.


#include "SpectatorPlayerPawn.h"

// Sets default values
ASpectatorPlayerPawn::ASpectatorPlayerPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASpectatorPlayerPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASpectatorPlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASpectatorPlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


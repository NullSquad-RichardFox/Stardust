// Fill out your copyright notice in the Description page of Project Settings.


#include "Connection.h"
#include "Stardust/GameModes/GameFramework.h"


// Sets default values
AConnection::AConnection()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	SelectedMaterial = CreateDefaultSubobject<UMaterialInterface>(TEXT("Path Material"));
}

void AConnection::CalculateMeshSize(float Dist)
{
	float Spacing = 20.f;

	SetActorScale3D(FVector(0.1 * Dist - 2 * Spacing, 15.f, 15.f));
}

AGameActor* AConnection::GetOtherActor(AGameActor* Actor)
{
	if (AGameFramework* GameMode = Cast<AGameFramework>(GetWorld()->GetAuthGameMode()))
	{
		int32 ActorIndex, StartIndex, EndIndex;

		if (GameMode->FindGameActor(Actor, ActorIndex) && GameMode->FindGameActor(StartActor, StartIndex) && GameMode->FindGameActor(EndActor, EndIndex))
		{
			return ActorIndex == StartIndex ? EndActor : StartActor;
		}
	}

	return nullptr;
}

void AConnection::Select()
{
	if (SelectedMaterial && Mesh)
	{
		UE_LOG(LogTemp, Warning, TEXT("Selected"))
		Mesh->SetMaterial(1, SelectedMaterial);
	}
}

void AConnection::SetActors(AGameActor* InStartActor, AGameActor* InEndActor)
{
	StartActor = InStartActor;
	EndActor = InEndActor;
}
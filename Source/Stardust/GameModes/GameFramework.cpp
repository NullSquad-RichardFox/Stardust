// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework.h"
#include "Stardust/Player/SpectatorPlayerController.h"
#include "Stardust/Player/SpectatorPlayerPawn.h"
#include "Stardust/Player/PlayerCorporation.h"
#include "Stardust/GameObjects/GameActor.h"
#include "Stardust/GameObjects/Connection.h"
#include "Stardust/GameObjects/Planet.h"
#include "Blueprint/UserWidget.h"
#include "Stardust/FindPath.h"



AGameFramework::AGameFramework()
{
	PrimaryActorTick.bCanEverTick = true;

	PlayerControllerClass = ASpectatorPlayerController::StaticClass();
	DefaultPawnClass = ASpectatorPlayerPawn::StaticClass();

	ConstructorHelpers::FClassFinder<APlanet> PlanetClassObject(TEXT("Blueprint'/Game/Blueprints/GameObjects/BP_Planet.BP_Planet_C'"));
	PlanetClass = PlanetClassObject.Class;

	ConstructorHelpers::FClassFinder<AConnection> ConnectionClassObject(TEXT("Blueprint'/Game/Blueprints/GameObjects/BP_Connection.BP_Connection_C'"));
	ConnectionClass = ConnectionClassObject.Class;

	ConstructorHelpers::FClassFinder<UUserWidget> MainUIClassObject(TEXT("WidgetBlueprint'/Game/Blueprints/UI/WBP_TimeUI.WBP_TimeUI_C'"));
	MainUIClass = MainUIClassObject.Class;
}

void AGameFramework::BeginPlay()
{
	Super::BeginPlay();

	if (MainUIClass)
		CreateWidget(GetWorld(), MainUIClass)->AddToViewport(999);

	GenerateMap(10);

	GetWorld()->SpawnActor<APlayerCorporation>();
	Pathfinder = UPathfinder(this);
}

void AGameFramework::DayUpdate()
{
	DayUpdateEvent.Broadcast();
}

void AGameFramework::MonthUpdate()
{
	MonthUpdateEvent.Broadcast();
}

void AGameFramework::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	PartialTimeDay += TimeTickAddition * TimeSpeedMultiplier * DeltaTime;

	if (PartialTimeDay >= 100)
	{
		PartialTimeDay -= 100;
		GameplayTime.X++;
		DayUpdate();
	}

	if (GameplayTime.X == 31)
	{
		GameplayTime.X = 1;
		GameplayTime.Y++;
		MonthUpdate();
	}

	if (GameplayTime.Y == 13)
	{
		GameplayTime.Y = 1;
		GameplayTime.Z++;
	}
}

void AGameFramework::GenerateMap(int32 PlanetCount)
{
	if (!PlanetClass || !ConnectionClass) return;

	const float HexagonRadius = 750.f;
	const float MaxPlanetConnectionLength = 2250.f;
	const int32 MaxPossibleConnections = 4;

	int32 Level = -0.5 + sqrt(0.25 + PlanetCount / 6 * 2);
	int32 PartialLayer = PlanetCount - 3 * Level * (Level + 1);

	GameActors.Reserve(PlanetCount);

	GenerateLayer<APlanet>(PlanetClass, Level + 1, PartialLayer, HexagonRadius);
	for (int32 i = 1; i < Level + 1; i++)
		GenerateLayer<APlanet>(PlanetClass, i, i * 6, HexagonRadius);

	//GenerateLayer<ATradePort>(ATradePort::StaticClass(), Level + 2, 4, HexagonRadius, false);

	GenerateConnections(MaxPlanetConnectionLength, MaxPossibleConnections);
}

template<typename T>
void AGameFramework::GenerateLayer(TSubclassOf<T> ActorClass, int32 LayerIndex, int32 ActorCount, float Radius, bool bRandom)
{
	const FVector Origin(0.f, 0.f, -150.f);
	const float Dist = sqrt(3) * Radius;
	const float Angle = bRandom ? PI / 3 / LayerIndex : 2 * PI / ActorCount;

	TArray<float> Angles;
	Angles.Reserve(bRandom ? LayerIndex * 6 : ActorCount);

	for (int32 j = 0; j < (bRandom ? LayerIndex * 6 : ActorCount); j++)
		Angles.Add(Angle * j);

	for (int32 j = 0; j < ActorCount; j++)
	{
		int32 AngleIndex = FMath::FRandRange<float>(0, Angles.Num() - 1);

		FMatrix2x2 RotationMatrix(	cos(Angles[AngleIndex]), -sin(Angles[AngleIndex]),
									sin(Angles[AngleIndex]), cos(Angles[AngleIndex]));

		FVector Displacement = FVector(FMath::FRandRange(-Radius / 1.9f, Radius / 1.9f), FMath::FRandRange(-Radius / 1.9f, Radius / 1.9f), FMath::FRandRange(-100.f, 100.f));
		FVector Position = FVector(RotationMatrix.TransformVector(FVector2D(Dist * LayerIndex, 0)), 0.f);
		FTransform Transform(FQuat(), Position + Displacement + Origin, FVector(1.f));

		T* Actor = GetWorld()->SpawnActor<T>(ActorClass, Transform);

		GameActors.Emplace((AGameActor*)Actor);
		Angles.RemoveAt(AngleIndex, 1, true);
	}
}

void AGameFramework::GenerateConnections(float MaxPlanetConnectionLength, int32 MaxPossibleConnections)
{
	constexpr float Exponent = 3.f;


	for (AGameActor* Actor : GameActors)
	{
		//Gets all connectable planets
		TArray<AGameActor*> AvailibleActors;
		for (AGameActor* OtherActor : GameActors)
		{
			if (OtherActor->Connections.Num() >= MaxPossibleConnections) continue;

			const float Dist = FVector(Actor->GetActorLocation() - OtherActor->GetActorLocation()).Length();

			if (Dist == 0) continue;
			if (Dist > MaxPlanetConnectionLength) continue;

			bool bAlreadyConnected = false;
			for (const AConnection* ActorConnection : Actor->Connections)
			{
				if (OtherActor->Connections.Contains(ActorConnection))
				{
					bAlreadyConnected = true;
					break;
				}
			}

			if (!bAlreadyConnected)
				AvailibleActors.Emplace(OtherActor);
		}

		for (int32 j = 0; j < MaxPossibleConnections; j++)
		{
			if (Actor->Connections.Num() >= MaxPossibleConnections) break;
			if (AvailibleActors.Num() == 0) break;

			const float Chance = pow<float>(1.f - (float)Actor->Connections.Num() / (float)MaxPossibleConnections, Exponent);
			if (FMath::FRand() > Chance) continue;



			int32 ChoosenActorIndex = FMath::FRandRange(0.f, (float)(AvailibleActors.Num() - 1));
			AGameActor* ChoosenActor = AvailibleActors[ChoosenActorIndex];

			FVector Dist(Actor->GetActorLocation() - ChoosenActor->GetActorLocation());
			FTransform Transform(Dist.Rotation(), ChoosenActor->GetActorLocation() + Dist / 2, FVector());

			AConnection* Connection = GetWorld()->SpawnActor<AConnection>(ConnectionClass, Transform);
			Connection->CalculateMeshSize(Dist.Length());
			Connection->SetActors(ChoosenActor, Actor);

			ChoosenActor->Connections.Emplace(Connection);
			Actor->Connections.Emplace(Connection);
			GameActorConnections.Emplace(Connection);
			AvailibleActors.RemoveAt(ChoosenActorIndex, 1, true);
		}
	}
}

bool AGameFramework::FindGameActor(AGameActor* Item, int32& Index)
{
	return GameActors.Find(Item, Index);
}

bool AGameFramework::FindActorConnection(AConnection* Connection, int32& Index)
{
	return GameActorConnections.Find(Connection, Index);
}

bool AGameFramework::IsValidGameActorIndex(int32 Index)
{
	return GameActors.IsValidIndex(Index);
}

AGameActor* AGameFramework::GetGameActorRef(int32 Index)
{
	return GameActors[Index];
}

int32 AGameFramework::GetGameActorNum()
{
	return GameActors.Num();
}

AConnection* AGameFramework::GetActorConnectionRef(int32 Index)
{
	return GameActorConnections[Index];
}

AGameFramework::UPathfinder& AGameFramework::GetPathfinder()
{
	return Pathfinder;
}

FString AGameFramework::GetDisplayTimeString()
{
	return FString::FromInt(GameplayTime.X) + ". " + FString::FromInt(GameplayTime.Y) + ". " + FString::FromInt(GameplayTime.Z);
}

void AGameFramework::SetTimeSpeedModifier(float Value)
{
	TimeSpeedMultiplier = FMath::Clamp(Value, 0, 100.f);
}



void AGameFramework::UPathfinder::ActorClicked(AGameActor* Actor)
{
	switch (InteractionStatus)
	{
	case FindingDestination:
		Nodes.Add(Actor);
		CreateRoute();
		InteractionStatus = AddingPathMidpoints;
		break;

	case AddingPathMidpoints:
		int32 Index;
		if (!Nodes.Find(Actor, Index))
		{
			Nodes.EmplaceAt(Nodes.Num() - 1, Actor);
			CreateRoute();
		}
		else if (Index != 0)
		{
			Nodes.Remove(Actor);

			if (Nodes.Num() == 1)
			{
				InteractionStatus = FindingDestination;
			}
			else
			{
				CreateRoute();
			}
		}
		break;
	}
}

void AGameFramework::UPathfinder::SetStartingActor(AGameActor* Actor)
{
	if (!GameModePtr) return;

	Nodes.Empty();
	Nodes.Add(Actor);

	ToggleActorClickeEvent(true);

	PathfinderEngine.SetGameModePtr(GameModePtr);
	InteractionStatus = FindingDestination;
}

void AGameFramework::UPathfinder::ToggleActorClickeEvent(bool Toggle)
{
	if (!GameModePtr) return;

	for (int32 i = 0; i < GameModePtr->GetGameActorNum(); i++)
	{
		GameModePtr->GetGameActorRef(i)->SetFindRouteActive(Toggle);
	}
}

void AGameFramework::UPathfinder::CreateRoute()
{
	PathfinderEngine.SetNodes(Nodes);
	PathfinderEngine.FindPath();
}
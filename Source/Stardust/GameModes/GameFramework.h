// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Stardust/FindPath.h"
#include "GameFramework.generated.h"



class APlanet;
class AConnection;
class AGameActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDailyTimerSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMonthlyTimerSignature);

UCLASS()
class STARDUST_API AGameFramework : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	class UPathfinder
	{
	private:
		typedef enum
		{
			FindingDestination,
			AddingPathMidpoints
		} InteractionType;

	public:
		UPathfinder()
			:GameModePtr(nullptr) {}

		UPathfinder(AGameFramework* GameModePtr)
			:GameModePtr(GameModePtr) {}

		void ActorClicked(AGameActor* Actor);
		void SetStartingActor(AGameActor* Actor);

		void ToggleActorClickeEvent(bool Toggle);

		void CreateRoute();

	private:
		TArray<AGameActor*> Nodes;
		AGameFramework* GameModePtr;
		FFindPath PathfinderEngine;
		InteractionType InteractionStatus;
	};

public:
	AGameFramework();

	bool FindGameActor(AGameActor* Item, int32& Index);
	bool FindActorConnection(AConnection* Connection, int32& Index);
	bool IsValidGameActorIndex(int32 Index);

	AGameActor* GetGameActorRef(int32 Index);
	int32 GetGameActorNum();

	AConnection* GetActorConnectionRef(int32 Index);

	UPathfinder& GetPathfinder();

	UFUNCTION(BlueprintCallable)
	FString GetDisplayTimeString();
	UFUNCTION(BlueprintCallable)
	void SetTimeSpeedModifier(float Value);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	virtual void DayUpdate();
	virtual void MonthUpdate();

private:
	void GenerateMap(int32 PlanetCount);

	template<typename T>
	void GenerateLayer(TSubclassOf<T> ActorClass, int32 LayerIndex, int32 ActorCount, float Radius, bool bRandom = true);
	void GenerateConnections(float MaxPlanetConnectionLength, int32 MaxPossibleConnections);

public:
	UPROPERTY(BlueprintAssignable, Category = "Time")
	FDailyTimerSignature DayUpdateEvent;
	UPROPERTY(BlueprintAssignable, Category = "Time")
	FMonthlyTimerSignature MonthUpdateEvent;

	const float PopulationGrowthThreshold = 100.f;
	const float PlanetStartingPopulationGrowth = 25.f;
	const int32 PlanetStartingPopulation = 5;
	const int32 PlanetStartingBuildStackCount = 1;

private:
	TArray<AGameActor*> GameActors;
	TArray<AConnection*> GameActorConnections;

	AGameActor* TradeRouteStartActor;
	UPathfinder Pathfinder;

	TSubclassOf<APlanet> PlanetClass;
	TSubclassOf<AConnection> ConnectionClass;
	TSubclassOf<UUserWidget> MainUIClass;

	FVector GameplayTime = FVector(1.f, 1.f, 2200.f);

	float TimeSpeedMultiplier = 1.f;
	float TimeTickAddition = 250.f;
	float PartialTimeDay = 0.f;
};
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Stardust/CoreDataStructs.h"
#include "Stardust/FindPath.h"
#include "GameFramework.generated.h"



class APlanet;
class AConnection;
class AGameActor;

DECLARE_DELEGATE(FGameTimerDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDailyTimerSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMonthlyTimerSignature);

USTRUCT()
struct FGameTimerData
{
	GENERATED_BODY()

public:
	FGameTimerData() {};
	FGameTimerData(int32 Rate, bool bLoop)
		:Rate(Rate), Time(Rate), bLoop(bLoop) {}

	void BindDelegate(UObject* Obj, FName FuncName) { Delegate.BindUFunction(Obj, FuncName); }
	void CallDelegate() { if (Delegate.IsBound()) Delegate.Execute(); }

	void SetTime(int32 Value) { Time = Value; }

	int32& GetTime() { return Time; }
	const int32 GetRate() { return Rate; }
	const bool CanLoop() { return bLoop; }

	const int32* GetTimePtr() const { return &Time; }

private:
	int32 Rate;
	int32 Time;
	bool bLoop;
	FGameTimerDelegate Delegate;
};

UCLASS()
class STARDUST_API AGameFramework : public AGameModeBase
{
	GENERATED_BODY()

public:
	AGameFramework();

	//Getter functions
	bool IsValidGameActorIndex(int32 Index) { return GameActors.IsValidIndex(Index); }
	bool FindGameActor(AGameActor* Item, int32& Index) { return GameActors.Find(Item, Index); }
	bool FindActorConnection(AConnection* Connection, int32& Index) { return GameActorConnections.Find(Connection, Index); }
	int32 GetGameActorNum() { return GameActors.Num(); }
	int32 GetActorConnectionNum() { return GameActorConnections.Num(); }
	AGameActor* GetGameActorRef(int32 Index) { return GameActors[Index]; }
	AConnection* GetActorConnectionRef(int32 Index) { return GameActorConnections[Index]; }

	UPathfinder& GetPathfinder() { return Pathfinder; }

	void SendTradeRoute(FTradeRoute TradeRoute);

	//Sets timer based on the in-game date system
	const FGameTimerData* SetTimer(UObject* Object, FName FunctionName, int32 NumDays, bool bLoop = false);

	//Sets the speed of the in-game time & events
	UFUNCTION(BlueprintCallable)
	void SetTimeSpeedModifier(float Value);

	//Stops the in-game date system 
	void PauseTime();

	//Returns the in-game date system to its previous speed
	void ResumeTime();

	UFUNCTION(BlueprintCallable)
	FString GetDisplayTimeString();

	UFUNCTION(BlueprintCallable)
	float GetPlayerMoney();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	//Updates every day, calls DayUpdateEvent
	virtual void DayUpdate();

	//Updates every month, calls MonthUpdateEvent
	virtual void MonthUpdate();

private:
	void GenerateMap(int32 PlanetCount);

	template<typename T>
	void GenerateLayer(TSubclassOf<T> ActorClass, int32 LayerIndex, int32 ActorCount, float Radius, bool bRandom = true);

	//Generates connections between game actors
	void GenerateConnections(float MaxPlanetConnectionLength, int32 MaxPossibleConnections);

	//Handles timer updates
	void UpdateTimers();

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
	TArray<FGameTimerData> Timers;

	AGameActor* TradeRouteStartActor;
	UPathfinder Pathfinder;

	TSubclassOf<APlanet> PlanetClass;
	TSubclassOf<AConnection> ConnectionClass;
	TSubclassOf<UUserWidget> MainUIClass;

	FVector GameplayTime = FVector(1.f, 1.f, 2200.f);

	const float TimeTickAddition = 250.f;
	float TimeSpeedMultiplier = 1.f;
	float TempTimeModifier = -1.f;
	float PartialTimeDay = 0.f;
};
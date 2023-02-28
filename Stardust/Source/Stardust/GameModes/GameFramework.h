// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Stardust/CoreDataStructs.h"
#include "Stardust/FindPath.h"
#include "GameFramework.generated.h"





#define LOG_LEVEL_MARKET 2

#if LOG_LEVEL_MARKET == 0
#define LOG_INFO(text, ...) 
#define LOG(text, ...) 
#define LOG_ERROR(text, ...)
#elif LOG_LEVEL_MARKET == 1
#define LOG_INFO(text, ...) 
#define LOG(text, ...) 
#define LOG_ERROR(text, ...) UE_LOG(LogTemp, Error, TEXT("[%i]: "##text), __LINE__, ##__VA_ARGS__)
#elif LOG_LEVEL_MARKET == 2
#define LOG_INFO(text, ...) 
#define LOG(text, ...) UE_LOG(LogTemp, Warning, TEXT("[%i]: "##text), __LINE__, ##__VA_ARGS__)
#define LOG_ERROR(text, ...) UE_LOG(LogTemp, Error, TEXT("[%i]: "##text), , __LINE__, ##__VA_ARGS__)
#elif LOG_LEVEL_MARKET == 3
#define LOG_INFO(text, ...) UE_LOG(LogTemp, Display, TEXT("[%i]: "##text), __LINE__, ##__VA_ARGS__)
#define LOG(text, ...) UE_LOG(LogTemp, Warning, TEXT("[%i]: "##text), __LINE__, ##__VA_ARGS__)
#define LOG_ERROR(text, ...) UE_LOG(LogTemp, Error, TEXT("[%i]: "##text), __LINE__, ##__VA_ARGS__)
#endif

#if LOG_LEVEL_MARKET == 0
#define IFLOG(con, text, ...) 
#define IFLOG_INFO(con, text, ...) 
#else
#define IFLOG(con, text, ...) if (con) \
{\
	LOG(text, ##__VA_ARGS__)	\
}

#define IFLOG_INFO(con, text, ...) if (con) \
{\
	LOG_INFO(text, ##__VA_ARGS__)	\
}
#endif


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

USTRUCT()
struct FWienerProcess
{
	GENERATED_BODY()

public:
	FWienerProcess()
		:ControllerDemand(0.f), ControllerSupply(0.f), WorldDemand(0.f), WorldSupply(0.f), Price(0.f), OriginalPrice(0.f), Complexity(0.f) {}

	FWienerProcess(double WorldDemand, double WorldSupply, float Price, EResourceType ResourceType, float Complexity)
		:ControllerDemand(0.f), ControllerSupply(0.f), WorldDemand(WorldDemand), WorldSupply(WorldSupply), OriginalWorldDemand(WorldDemand), OriginalWorldSupply(WorldSupply), ResourceType(ResourceType), Price(Price), OriginalPrice(Price), Complexity(Complexity) {}

	void UpdatePrice(FVector GameDate)
	{
		IFLOG_INFO(ResourceType == EResourceType::Ore, "Ore price: %f", (float)Price);
		IFLOG_INFO(ResourceType == EResourceType::Ore, "Ore mean: %f", (float)Mean);

		float Mean = (WorldDemand - WorldSupply) * SupplyDemandMeanWeight + (ControllerDemand - ControllerSupply) * PlayerSupplyDemandWeight;
		Price *= (GaussianRandom(Mean, 0.25) * PriceChangeMultiplier) + 1;

		IFLOG(Price < OriginalPrice * 0.35f, "Price is too low - %f", Price)
		IFLOG(Mean > 1.1 || Mean < -1.1, "Mean is out of bounds - %f", Mean)
		
		WorldDemand = OriginalWorldDemand;
		WorldSupply = OriginalWorldSupply;

		AddNewSupplyDemand();
		IFLOG_INFO(ResourceType == EResourceType::Ore, "NEW Supply: %f, Demand: %f", WorldSupply, WorldDemand);


		PriceData.Add(Price);
	}

	double GetPrice()
	{
		return Price;
	}

	const TArray<double>& GetPriceData()
	{
		return PriceData;
	}

private:
	double GaussianRandom(float Mean, float StandDev) const
	{
		double u1 = 1.0 - (double)FMath::RandHelper64(RAND_MAX) / RAND_MAX;
		double u2 = 1.0 - (double)FMath::RandHelper64(RAND_MAX) / RAND_MAX;

		double randStdMormal = FMath::Sqrt(-2.0 * FMath::Loge(u1)) * FMath::Cos(2.0 * PI * u2);

		double randNormal = Mean + StandDev * randStdMormal;

		return randNormal;
	}

	void AddNewSupplyDemand()
	{
		if (Price <= 0) return;

		constexpr float DemandMultiplier = 11.5f;
		constexpr float SupplyMultiplier = 11.5f;

		const float PriceFloorMultiplier = OriginalPrice / (2 - Complexity);
		const float PriceSeilingMultiplier = 1.35f;


		float OneOver = PriceFloorMultiplier / Price / Price;
		float DemandFinal = OneOver * DemandMultiplier;

		float Value = 4 * Price / OriginalPrice - 6 * PriceSeilingMultiplier;
		float Eqv = FMath::Clamp(0.5 * Value * Value * Value + 2.2 * Value * Value + 3.3 * Value + 5 - 7.5 * Complexity, 0, MAX_FLT);
		float SupplyFinal = Eqv * SupplyMultiplier;

		WorldDemand += DemandFinal;
		WorldSupply += SupplyFinal;
	}

private:
	// Weights
	float PriceChangeMultiplier = 0.05f;
	float SupplyDemandMeanWeight = 0.075f;
	float PlayerSupplyDemandWeight = 0.005f;

	// Wiener
	// Tells how complex the resource is this has inpact on how the market responds to the changes in price
	float Complexity;

	float ControllerDemand;
	float ControllerSupply;

	double WorldDemand; // 100 000 000
	double WorldSupply; // 100 000 000

	// Used for reseting Supply and Demand values
	double OriginalWorldDemand;
	double OriginalWorldSupply;

	// Type of the resource 
	EResourceType ResourceType;

	// Actual price of the resource
	double Price;

	// Used to anchor the price 
	double OriginalPrice;

	// Stores the price data
	TArray<double> PriceData;
	

	// TODO: Correlation
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


	// Pathfinding + trade routes
	UPathfinder& GetPathfinder() { return Pathfinder; }
	void SendTradeRoute(FTradeRoute TradeRoute);

	void StockMarket();
	void SetUpWienerProcesses();

	// ---------------------
	// 
	//  In-Game Time System
	// 
	// ---------------------

	//Sets timer based on the in-game date system
	const FGameTimerData* SetTimer(UObject* Object, FName FunctionName, int32 NumDays, bool bLoop = false);

	//Sets the speed of the in-game time & events
	UFUNCTION(BlueprintCallable)
	void SetTimeSpeedModifier(float Value);

	//Stops the in-game date system 
	UFUNCTION(BlueprintCallable)
	void PauseTime();

	//Returns the in-game date system to its previous speed
	UFUNCTION(BlueprintCallable)
	void ResumeTime();

	UFUNCTION(BlueprintCallable)
	bool IsTimePaused();

	UFUNCTION(BlueprintCallable)
	FString GetDisplayTimeString();

	UFUNCTION(BlueprintCallable)
	float GetPlayerMoney();

	UFUNCTION(BlueprintCallable)
	int32 GetResourcePrice(EResourceType Resource);

	UFUNCTION(BlueprintCallable)
	const TArray<double> GetResourcePriceData(EResourceType Resource);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	//Updates every day, calls DayUpdateEvent
	virtual void DayUpdate();

	//Updates every month, calls MonthUpdateEvent
	virtual void MonthUpdate();

private:
	/* Generates game map given the size
	*  @param PlanetCount - Map size in total planets
	*/
	void GenerateMap(int32 PlanetCount);

	/* Creates hexagonal map layer
	*  @param ActorClass - AGameActor child class 
	*/
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
	TMap<EResourceType, FWienerProcess> ResourcePriceProcesses;

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
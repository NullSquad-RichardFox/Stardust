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
#define LOG_ERROR(text, ...) UE_LOG(LogTemp, Error, TEXT("[%s][%i]: "##text), *GetName(), __LINE__, ##__VA_ARGS__)
#elif LOG_LEVEL_MARKET == 1
#define LOG_INFO(text, ...) 
#define LOG(text, ...) UE_LOG(LogTemp, Warning, TEXT("[%s][%i]: "##text), *GetName(), __LINE__, ##__VA_ARGS__)
#define LOG_ERROR(text, ...) UE_LOG(LogTemp, Error, TEXT("[%s][%i]: "##text), *GetName(), __LINE__, ##__VA_ARGS__)
#elif LOG_LEVEL_MARKET == 2
#define LOG_INFO(text, ...) UE_LOG(LogTemp, Display, TEXT("[%i]: "##text), __LINE__, ##__VA_ARGS__)
#define LOG(text, ...) UE_LOG(LogTemp, Warning, TEXT("[%s][%i]: "##text), *GetName(), __LINE__, ##__VA_ARGS__)
#define LOG_ERROR(text, ...) UE_LOG(LogTemp, Error, TEXT("[%s][%i]: "##text), *GetName(), __LINE__, ##__VA_ARGS__)
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
		:ControllerDemand(0.f), ControllerSupply(0.f), WorldDemand(0.f), WorldSupply(0.f), Change(0.f), Price(0.f), OriginalPrice(0.0), CorrelatedProcesses({}) {}

	FWienerProcess(float WorldDemand, float WorldSupply, float Price, EResourceType ResourceType)
		:ControllerDemand(0.f), ControllerSupply(0.f), WorldDemand(WorldDemand), WorldSupply(WorldSupply), Change(0.f), Price(Price), OriginalPrice(Price), ResourceType(ResourceType), CorrelatedProcesses({}) {}

	void UpdatePrice()
	{
		float Demand = ControllerDemand + WorldDemand;
		float Supply = WorldSupply + ControllerSupply;
		float Mean = (Demand - Supply) * SupplyDemandMultiplier + Inflation * InflationImpactMultiplier;

		if (ResourceType == EResourceType::Ore)
			LOG_INFO("Mean: [%f], Price: [%f], S: [%f] D: [%f]", Mean, Price, Supply, Demand)

		Change = GaussianRandom(Mean, 1 - Volatility) * PriceChangeMultiplier;
		Price += Change;
	}

	// Call only after all price updates are executed
	void UpdateCorrelations() 
	{
		if (CorrelatedProcesses.Num() == 0)
		{
			AdjustSupplyDemand();
			return;
		}

		double Difference = 0;
		for (const auto& [Wiener, Rho] : CorrelatedProcesses)
		{
			if (!Wiener)
				continue;

			Difference += (Wiener->GetPrice() - Price) * Rho;
		}

		//Difference *= CorrelationMultiplier;

		double CorrelationChange = GaussianRandom(Difference, 1 - Volatility) * CorrelationMultiplier;
		Change += CorrelationChange;
		Price += CorrelationChange;

		if (ResourceType == EResourceType::Ore)
			LogChange();

		AdjustSupplyDemand();
	}

	FORCEINLINE void SetControllerSupply(float InVal) 
	{
		ControllerSupply = InVal;
	}

	FORCEINLINE void SetControllerDemand(float InVal)
	{
		ControllerDemand = InVal;
	}

	FORCEINLINE void SetWorldSupply(float InVal)
	{
		WorldSupply = InVal;
		if (ResourceType == EResourceType::Ore)
		{
			LOG_INFO("%f", WorldSupply)
		}
	}

	FORCEINLINE void SetWorldDemand(float InVal)
	{
		WorldDemand = InVal;
		if (ResourceType == EResourceType::Ore)
		{
			LOG_INFO("%f", WorldDemand)
		}
	}

	float GetPrice() const
	{
		return (float)Price;
	}

	void AddCorrelation(const FWienerProcess& Process, float Rho)
	{
		CorrelatedProcesses.Add(&Process, Rho);
	}

private:
	void LogChange()
	{
		UEnum* ResourceEnum = FindObject<UEnum>(ANY_PACKAGE, TEXT("EResourceType"), true);
		if (!ResourceEnum) return;

		ChangeData.Add(Change);

		if (ChangeData.Num() > 30)
			ChangeData.RemoveAt(0);

		float Sum = 0;
		
		for (float C : ChangeData)
			Sum += C;

		Sum /= ChangeData.Num();
		if (ResourceType == EResourceType::Ore)
		{
			LOG_INFO("Res change - Avr. Change: [%f], ResType: [%s]", Sum, *ResourceEnum->GetNameStringByValue((int32)ResourceType))
		}
	}

	double GaussianRandom(float Mean, float StandDev) const
	{
		double u1 = 1.0 - (double)FMath::RandHelper64(RAND_MAX) / RAND_MAX;
		double u2 = 1.0 - (double)FMath::RandHelper64(RAND_MAX) / RAND_MAX;

		double randStdMormal = FMath::Sqrt(-2.0 * FMath::Loge(u1)) * FMath::Sin(2.0 * PI * u2);

		double randNormal = Mean + StandDev * randStdMormal;

		return randNormal;
	}

	double SigmoidFunction(double Alpha)
	{
		double Base = 0.000001;
		double XOffset = 0.5;

		double Denominator = 1 + pow(Base, XOffset - Alpha);

		return 1 / Denominator;
	}

	void AdjustSupplyDemand()
	{
		float Modifier = 2.f;

		float Alpha = Price / OriginalPrice / 0.2;
		LOG_INFO("Alpha is: %f", Alpha);

		if (Alpha <= 0)
			Alpha = 0.000000001;

		WorldDemand -= 0.01 * pow(OldAlpha, -2) * Modifier;
		WorldDemand += 0.01 * pow(Alpha, -2) * Modifier;

		OldAlpha = Alpha;
	}

private:

	// Market
	float Volatility = 0.0f;
	float Inflation = 0.0f;

	// Weights
	float InflationImpactMultiplier = 0.0f;
	float SupplyDemandMultiplier = 0.1f;
	float PriceChangeMultiplier = 0.5f;
	float CorrelationMultiplier = 0.0f;

	// Wiener

	float ControllerDemand;
	float ControllerSupply;

	float WorldDemand;
	float WorldSupply;

	double Change;
	double Price;

	double OriginalPrice;
	float OldAlpha;

	EResourceType ResourceType;

	TMap<const FWienerProcess*, float> CorrelatedProcesses;
	TArray<float> ChangeData;
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
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
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
	AGameFramework();

	bool FindGameActor(AGameActor* Item, int32& Index);

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

private:
	TArray<AGameActor*> GameActors;
	TArray<AConnection*> GameActorConnections;

	TSubclassOf<APlanet> PlanetClass;
	TSubclassOf<AConnection> ConnectionClass;

	FVector GameplayTime = FVector(1.f, 1.f, 2200.f);

	float TimeSpeedMultiplier = 1.f;
	float TimeTickAddition = 250.f;
	float PartialTimeDay = 0.f;
};

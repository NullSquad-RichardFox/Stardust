// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Stardust/CoreDataStructs.h"
#include "ListDataLibrary.generated.h"



UCLASS(BlueprintType)
class STARDUST_API UDistrictListData : public UObject
{
	GENERATED_BODY()

public:
	void MakeDistrictData(int32 BuildSlotIndex, EDistrictType DistrictType, AActor* OwningActor)
	{
		InBuildSlotIndex = BuildSlotIndex;
		InDistrictType = DistrictType;
		InOwningActor = OwningActor;
	}

	UFUNCTION(BlueprintCallable)
	void GetDistrictData(int32& BuildSlotIndex, EDistrictType& DistrictType, AActor*& OwningActor)
	{
		BuildSlotIndex = InBuildSlotIndex;
		DistrictType = InDistrictType;
		OwningActor = InOwningActor;
	}

private:
	int32 InBuildSlotIndex;
	EDistrictType InDistrictType;
	AActor* InOwningActor;
};

UCLASS(BlueprintType)
class STARDUST_API UBuildingListData : public UObject
{
	GENERATED_BODY()

public:
	void MakeBuildingData(int32 BuildSlotIndex, EBuildingType BuildingType, AActor* OwningActor)
	{
		InBuildSlotIndex = BuildSlotIndex;
		InBuildingType = BuildingType;
		InOwningActor = OwningActor;
	}

	UFUNCTION(BlueprintCallable)
	void GetBuildingData(int32& BuildSlotIndex, EBuildingType& BuildingType, AActor*& OwningActor)
	{
		BuildSlotIndex = InBuildSlotIndex;
		BuildingType = InBuildingType;
		OwningActor = InOwningActor;
	}

private:
	int32 InBuildSlotIndex;
	EBuildingType InBuildingType;
	AActor* InOwningActor;
};

UCLASS(BlueprintType)
class STARDUST_API UJobListData : public UObject
{
	GENERATED_BODY()

public:
	void MakeJobData(EJobType JobType, int32 TotalJobs, int32 OccupiedJobs, int32 BuildSlotIndex, AActor* OwningActor)
	{
		InJobType = JobType;
		InTotalJobs = TotalJobs;
		InOccupiedJobs = OccupiedJobs;
		InBuildSlotIndex = BuildSlotIndex;
		InOwningActor = OwningActor;
	}

	UFUNCTION(BlueprintCallable)
	void GetJobData(EJobType& JobType, int32& TotalJobs, int32& OccupiedJobs, int32& BuildSlotIndex, AActor*& OwningActor)
	{
		JobType = InJobType;
		TotalJobs = InTotalJobs;
		OccupiedJobs = InOccupiedJobs;
		BuildSlotIndex = InBuildSlotIndex;
		OwningActor = InOwningActor;
	}

private:
	EJobType InJobType;
	int32 InTotalJobs, InOccupiedJobs, InBuildSlotIndex;
	AActor* InOwningActor;
};

UCLASS(BlueprintType)
class STARDUST_API UResourceFlowListData : public UObject
{
	GENERATED_BODY()

public:
	void MakeResourceFlowData(float ResourceAmount, EResourceType ResourceType)
	{
		InResourceAmount = ResourceAmount;
		InResourceType = ResourceType;
	}

	UFUNCTION(BlueprintCallable)
	void GetResourceFlowData(float& ResourceAmount, EResourceType& ResourceType)
	{
		ResourceAmount = InResourceAmount;
		ResourceType = InResourceType;
	}

private:
	float InResourceAmount;
	EResourceType InResourceType;
};

UCLASS(BlueprintType)
class STARDUST_API UResourceListData : public UObject
{
	GENERATED_BODY()

public:
	void MakeResourceData(EResourceType ResourceType, float ResourceAmount, float ResourceChange)
	{
		InResourceType = ResourceType;
		InResourceAmount = ResourceAmount;
		InResourceChange = ResourceChange;
	}

	UFUNCTION(BlueprintCallable)
	void GetResourceData(EResourceType& ResourceType, float& ResourceAmount, float& ResourceChange)
	{
		ResourceType = InResourceType;
		ResourceAmount = InResourceAmount;
		ResourceChange = InResourceChange;
	}

private:
	EResourceType InResourceType;
	float InResourceAmount, InResourceChange;
};

UCLASS(BlueprintType)
class STARDUST_API UTradeRouteListData : public UObject
{
	GENERATED_BODY()

public:
	void MakeTradeRouteData(FString StartingPlantName, FString TargetPlanetName, int32 DangerLevel, const int32* TimeDays, float FuelConsumption, const TMap<EResourceType, float>& Payload)
	{
		InStartingPlantName = StartingPlantName;
		InTargetPlanetName = TargetPlanetName;
		InDangerLevel = DangerLevel;
		InTimeDays = TimeDays;
		InFuelConsumption = FuelConsumption;
		InPayload = Payload;
	}

	void GetTradeRouteData(FString& StartingPlantName, FString& TargetPlanetName, int32& DangerLevel, const int32*& TimeDays, float& FuelConsumption, TMap<EResourceType, float>& Payload)
	{
		StartingPlantName = InStartingPlantName;
		TargetPlanetName = InTargetPlanetName;
		DangerLevel = InDangerLevel;
		TimeDays = InTimeDays;
		FuelConsumption = InFuelConsumption;
		Payload = InPayload;
	}

private:
	FString InStartingPlantName, InTargetPlanetName;
	int32 InDangerLevel;
	const int32* InTimeDays;
	float InFuelConsumption;
	TMap<EResourceType, float> InPayload;
};

UCLASS(BlueprintType)
class STARDUST_API UBuildQueueListData : public UObject
{
	GENERATED_BODY()

public:
	void MakeBuildQueueData(FText BuildingName, const float* BuildTime, float TotalBuildTime, int32 QueueIndex, AActor* OwningActor)
	{
		InBuildingName = BuildingName;
		InBuildTime = BuildTime;
		InTotalBuildTime = TotalBuildTime;
		InQueueIndex = QueueIndex;
		InOwningActor = OwningActor;
	}

	void GetBuildQueueData(FText& BuildingName, const float*& BuildTime, float& TotalBuildTime, int32& QueueIndex, AActor*& OwningActor)
	{
		BuildingName = InBuildingName;
		BuildTime = InBuildTime;
		TotalBuildTime = InTotalBuildTime;
		QueueIndex = InQueueIndex;
		OwningActor = InOwningActor;
	}

private:
	FText InBuildingName;
	float InTotalBuildTime;
	AActor* InOwningActor;
	int32 InQueueIndex;
	const float* InBuildTime;
};
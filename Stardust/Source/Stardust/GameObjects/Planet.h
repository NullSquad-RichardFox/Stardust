// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameActor.h"
#include "Stardust/CoreDataStructs.h"
#include "Planet.generated.h"


class UCameraComponent;
class UPlanetWidget;
class APlayerCorporation;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBuildingFinishedSignature, int32, BuildSlotIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPopulationChangedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTradeRouteChangedSignature);

enum class EBuildingStatus
{
	Building,
	InQueue,
	Completed
};

UCLASS()
class STARDUST_API UBuildRequest : public UObject
{
	GENERATED_BODY()

public:
	UBuildRequest()
		: BuildSlotIndex(-1), BuildTime(-1), BuildCost(-1), Status(), Type() {}

	UBuildRequest(int32 BuildSlotIndex, float BuildTime, float BuildCost, EBuildingStatus Status, TVariant<EBuildingType, EDistrictType> Type)
		: BuildSlotIndex(BuildSlotIndex), BuildTime(BuildTime), BuildCost(BuildCost), Status(Status), Type(Type) {}

	int32 BuildSlotIndex;
	float BuildTime;
	float BuildCost;
	EBuildingStatus Status;
	TVariant<EBuildingType, EDistrictType> Type;
};

UCLASS()
class STARDUST_API APlanet : public AGameActor
{
	GENERATED_BODY()

public:
	APlanet();

	// -- Building --
	const UBuildRequest* BuildBuilding(int32 BuildSlotIndex, EBuildingType BuildingType);
	const UBuildRequest* BuildDistrict(int32 BuildSlotIndex, EDistrictType DistrictType);
	const UBuildRequest* UpgradeDistrict(int32 BuildSlotIndex);
	int32 DowngradeDistrict(int32 BuildSlotIndex);
	void CancelBuildingProcess(int32 RequestIndex);

	bool ColonizePlanet(APlayerCorporation* InCorporation);
	void TradeRouteSent(const FTradeRoute& TradeRoute);
	void TradeRouteFinished(const FTradeRoute& TradeRoute);

	bool FreeJob(int32 BuildSlotIndex, EJobType JobType);
	bool PopulateJob(int32 BuildSlotIndex, EJobType JobType);
	bool PopulateJob(int32 BuildSlotIndex);

	const FBuildSlot& GetBuildSlot(int32 Index);
	const TMap<EResourceType, float>& GetResourceStorage();
	const TMap<EResourceType, float>& GetResourceProduction();
	const TMap<EResourceType, float>& GetResourceUpkeep();
	TMap<EJobType, int32> GetJobs();
	TMap<EJobType, int32> GetOccupiedJobs();
	const TArray<FTradeRoute>& GetTradeRoutes();

	bool IsValidBuildSlotIndex(int32 BuildSlotIndex);

protected:
	virtual void BeginPlay() override;
	virtual void OnDayUpdate() override;
	virtual void OnMonthUpdate() override;
	virtual void OnClicked() override;

private:
	void CreatePlanet();
	void GenerateFeature(int32 FeatureIndex, FBuildSlot& BuildSlot);
	void GeneratePlanetaryFeature(int32 FeatureIndex);

	void HandleBuilding();
	void RecalculateProduction();
	void RecalculateEnergy();
	void RecalculateModifiers();
	void OccupyJobs();

public:
	UPROPERTY()
	FBuildingFinishedSignature BuildingFinishedEvent;
	UPROPERTY()
	FPopulationChangedSignature PopulationChangedEvent;
	UPROPERTY()
	FTradeRouteChangedSignature TradeRouteChangedEvent;

protected:
	UPROPERTY(EditAnywhere)
		UMaterialInterface* TerrestrialMaterial;
	UPROPERTY(EditAnywhere)
		UMaterialInterface* OceanMaterial;
	UPROPERTY(EditAnywhere)
		UMaterialInterface* BarrenMaterial;

private:
	friend UPlanetWidget;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TSubclassOf<UPlanetWidget> PlanetWidgetClass;

	UPlanetWidget* PlanetWidget;

	UPROPERTY(EditAnywhere)
	UCameraComponent* WidgetCamera;

	UPROPERTY(VisibleAnywhere, Category = "Infrastructure")
	int32 MaxBuildStackCount;
	UPROPERTY(VisibleAnywhere, Category = "Infrastructure")
	TArray<FBuildSlot> BuildSlots;

	UPROPERTY(VisibleAnywhere, Category = "Infrastructure|Population")
	int32 Population;
	UPROPERTY(VisibleAnywhere, Category = "Infrastructure|Population")
	float PopulationGrowth;
	UPROPERTY(VisibleAnywhere, Category = "Infrastructure|Population")
	float PopulationGrowthThreshold = MAX_FLT;

	UPROPERTY(VisibleAnywhere, Category = "Resources")
	TMap<EResourceType, float> ResourceProduction;
	UPROPERTY(VisibleAnywhere, Category = "Resources")
	TMap<EResourceType, float> ResourceUpkeep;
	UPROPERTY(VisibleAnywhere, Category = "Resources")
	TMap<EResourceType, float> ResourceStorage;

	UPROPERTY(VisibleAnywhere, Category = "Resources|Energy")
	float EnergyFinal;
	UPROPERTY(VisibleAnywhere, Category = "Resources|Energy")
	float EnergyProduction;
	UPROPERTY(VisibleAnywhere, Category = "Resources|Energy")
	float EnergyConsumption;

	UPROPERTY(VisibleAnywhere, Category = "Planet")
	int32 PlanetSize;
	UPROPERTY(VisibleAnywhere, Category = "Planet")
	EPlanetType PlanetType;
	UPROPERTY(VisibleAnywhere, Category = "Planet")
	TArray<EFeatureType> PlanetaryFeatures;
	UPROPERTY(VisibleAnywhere, Category = "Planet")
	TArray<EPlanetModifier> PlanetaryModifiers;

	TArray<UBuildRequest*> BuildQueue;
	TMap<EResourceType, float> UpkeepModifiers;
	TMap<EResourceType, float> ProductionModifiers;
	TArray<FTradeRoute> TradeRoutes;

	float PartialPopulation;
	float UnemployedPopulation;
	float PopulationFoodConsumption = 1.5f;
};
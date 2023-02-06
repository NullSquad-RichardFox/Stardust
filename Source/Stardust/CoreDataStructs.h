// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "CoreDataStructs.generated.h"


class AConnection;
class AGameActor;

UENUM()
enum class EResourceType
{
	Oxygen					UMETA(DisplayName = "Oxygen"),
	Water					UMETA(DisplayName = "Water"),
	Food					UMETA(DisplayName = "Food"),
	Ore						UMETA(DisplayName = "Ore"),
	Metals					UMETA(DisplayName = "Metals"),
	Sand					UMETA(DisplayName = "Sand"),
	Silicon					UMETA(DisplayName = "Silicon"),
	Carbon					UMETA(DisplayName = "Carbon"),
	Glass					UMETA(DisplayName = "Glass"),
	Electronics				UMETA(DisplayName = "Electronics")
};

UENUM()
enum class EPlanetType : uint8
{
	Ocean					UMETA(DisplayName = ""),
	Barren					UMETA(DisplayName = ""),
	Terrestrial				UMETA(DisplayName = "")
};

UENUM()
enum class EFeatureType : uint8
{
	Default					UMETA(DisplayName = ""),
	MetalDeposits			UMETA(DisplayName = ""),
	CarbonReserves			UMETA(DisplayName = ""),
	SpralingShores			UMETA(DisplayName = ""),
	DeepOceans				UMETA(DisplayName = ""),
	DenseForests			UMETA(DisplayName = "")
};

UENUM()
enum class EBuildingType : uint8
{
	Greenhouse				UMETA(DisplayName = ""),
	SolarPanels				UMETA(DisplayName = ""),
	WindTurbines			UMETA(DisplayName = ""),
	Bioreactor				UMETA(DisplayName = ""),
	ElectrolysisReactor		UMETA(DisplayName = ""),
	AdditionalMiningShafts	UMETA(DisplayName = ""),
	ExtraFactorySpaces		UMETA(DisplayName = ""),
	SecondaryReactorChamber	UMETA(DisplayName = ""),
	TheatreSquare			UMETA(DisplayName = ""),
	Zoo						UMETA(DisplayName = ""),
	Aquarium				UMETA(DisplayName = "")
};

UENUM()
enum class EJobType : uint8
{
	Forger					UMETA(DisplayName = ""),
	GlassWorker				UMETA(DisplayName = ""),
	SiliconWorker			UMETA(DisplayName = ""),
	ElectronicsWorker		UMETA(DisplayName = ""),
	Farmer					UMETA(DisplayName = ""),
	OreMiner				UMETA(DisplayName = ""),
	CarbonMiner				UMETA(DisplayName = ""),
	SandMiner				UMETA(DisplayName = ""),
	WaterMiner				UMETA(DisplayName = ""),
	PhysicsEngineer			UMETA(DisplayName = ""),
	MaintenanceWorker		UMETA(DisplayName = ""),
	ElectrolysisEngineer	UMETA(DisplayName = ""),
	Actor					UMETA(DisplayName = ""),
	AnimalKeeper			UMETA(DisplayName = ""),
	Administrator			UMETA(DisplayName = ""),
	DistrictDefault			UMETA(DisplayName = "")
};

UENUM()
enum class EDistrictType : uint8
{
	OreCollection			UMETA(DisplayName = ""),
	CarbonCollection		UMETA(DisplayName = ""),
	SandCollection			UMETA(DisplayName = ""),
	WaterCollection			UMETA(DisplayName = ""),
	FoodCollection			UMETA(DisplayName = ""),
	MetalsManufacture		UMETA(DisplayName = ""),
	GlassManufacture		UMETA(DisplayName = ""),
	SiliconManufacture		UMETA(DisplayName = ""),
	ElectronicsManufacture	UMETA(DisplayName = ""),
	PowerProduction			UMETA(DisplayName = "")
};

UENUM()
enum class EDistrictModifier : uint8
{
	Mod						UMETA(DisplayName = "")
};

UENUM()
enum class EPlanetModifier : uint8
{
	Mod						UMETA(DisplayName = "")
};





UCLASS()
class STARDUST_API UStructDataLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UStructDataLibrary();

	static const FPlanetSizeRangeData* GetData(EPlanetType PlanetType);
	static const FBuildingData* GetData(EBuildingType BuildingType);
	static const FDistrictData* GetData(EDistrictType DistrictType, int32 DistrictTier);
	static const FJobData* GetData(EJobType JobType);
	static const FModifierData* GetData(EDistrictModifier ModifierType);
	static const FModifierData* GetData(EPlanetModifier ModifierType);
	static const FFeatureData* GetData(EFeatureType FeatureType);

private:
	UDataTable* LoadDataTable(const FName& FilePath);

	static UDataTable* PlanetSizeRangeDataTable;
	static UDataTable* DistrictDataTable;
	static UDataTable* BuildingDataTable;
	static UDataTable* JobDataTable;
	static UDataTable* DistrictModifiersDataTable;
	static UDataTable* PlanetModifiersDataTable;
	static UDataTable* FeatureDataTable;
};

UDataTable* UStructDataLibrary::PlanetSizeRangeDataTable = nullptr;
UDataTable* UStructDataLibrary::BuildingDataTable = nullptr;
UDataTable* UStructDataLibrary::DistrictDataTable = nullptr;
UDataTable* UStructDataLibrary::JobDataTable = nullptr;
UDataTable* UStructDataLibrary::DistrictModifiersDataTable = nullptr;
UDataTable* UStructDataLibrary::PlanetModifiersDataTable = nullptr;
UDataTable* UStructDataLibrary::FeatureDataTable = nullptr;





USTRUCT()
struct FPlanetSizeRangeData : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Construction")
	FVector2D Range;
};

USTRUCT()
struct FDistrictData : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Construction")
		float BuildTime;
	UPROPERTY(EditAnywhere, Category = "Construction")
		float BuildCost;
	UPROPERTY(EditAnywhere, Category = "Infrastructure")
		float EnergyProduction;
	UPROPERTY(EditAnywhere, Category = "Infrastructure")
		float EnergyUpkeep;
	UPROPERTY(EditAnywhere, Category = "Infrastructure")
		int32 BuildSlots;
	UPROPERTY(EditAnywhere, Category = "Jobs")
		TMap<EJobType, int32> DistrictJobs;
};

USTRUCT()
struct FBuildingData : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Construction")
		TArray<EDistrictType> RequiredDistrict;
	UPROPERTY(EditAnywhere, Category = "Construction")
		float BuildTime;
	UPROPERTY(EditAnywhere, Category = "Construction")
		float BuildCost;
	UPROPERTY(EditAnywhere, Category = "Infrastructure")
		float EnergyProduction;
	UPROPERTY(EditAnywhere, Category = "Infrastructure")
		float EnergyUpkeep;
	UPROPERTY(EditAnywhere, Category = "Infrastructure")
		TMap<EResourceType, float> ResourceUpkeep;
	UPROPERTY(EditAnywhere, Category = "Jobs")
		TMap<EJobType, int32> BuildingJobs;
};

USTRUCT()
struct FJobData : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
		TMap<EResourceType, float> ResourceProduction;
	UPROPERTY(EditAnywhere)
		TMap<EResourceType, float> ResourceUpkeep;
};

USTRUCT()
struct FModifierData : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
		TMap<EResourceType, float> ProductionModifiers;
	UPROPERTY(EditAnywhere)
		TMap<EResourceType, float> UpkeepModifiers;
};

USTRUCT()
struct FFeatureData : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
		bool bPlanetary;
	UPROPERTY(EditAnywhere)
		bool bMultiple;

	UPROPERTY(EditAnywhere)
		float Rarity;

	UPROPERTY(EditAnywhere)
		FVector2D RequiredSize;
	UPROPERTY(EditAnywhere)
		TArray<EPlanetType> RequiredType;
	UPROPERTY(EditAnywhere)
		TArray<EFeatureType> Prerequisites;

	UPROPERTY(EditAnywhere)
		TArray<EDistrictType> BuildableDistricts;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "bPlanetary"))
		TArray<EPlanetModifier> PlanetModifiers;
	UPROPERTY(EditAnywhere, meta = (EditCondition = "!bPlanetary"))
		TArray<EDistrictModifier> DistrictModifiers;
};

USTRUCT()
struct FDistrict
{
	GENERATED_BODY()

public:
	FDistrict()
		:DistrictTier(0),
		DistrictType(),
		EnergyProduction(0.f),
		EnergyUpkeep(0.f),
		BuildSlots(0),
		DistrictJobs({}),
		Buildings({}),
		ResourceModifiers({})
	{}

	FDistrict(int32 DistrictTier, EDistrictType DistrictType, float EnergyProduction, float EnergyUpkeep, int32 BuildSlots, TMap<EJobType, int32> DistrictJobs, TArray<EBuildingType> Buildings)
		:DistrictTier(DistrictTier),
		DistrictType(DistrictType),
		EnergyProduction(EnergyProduction),
		EnergyUpkeep(EnergyUpkeep),
		BuildSlots(BuildSlots),
		DistrictJobs(DistrictJobs),
		Buildings(Buildings),
		ResourceModifiers({})
	{}

	FDistrict(int32 DistrictTier, EDistrictType DistrictType, float EnergyProduction, float EnergyUpkeep, int32 BuildSlots, TMap<EJobType, int32> DistrictJobs, TArray<EBuildingType> Buildings, TArray<EDistrictModifier> ResourceModifiers)
		:DistrictTier(DistrictTier),
		DistrictType(DistrictType),
		EnergyProduction(EnergyProduction),
		EnergyUpkeep(EnergyUpkeep),
		BuildSlots(BuildSlots),
		DistrictJobs(DistrictJobs),
		Buildings(Buildings),
		ResourceModifiers(ResourceModifiers)
	{}

	TMap<EResourceType, float> GetDistrictProduction() const;
	TMap<EResourceType, float> GetDistrictUpkeep() const;

	void PopulateJob(EJobType Job);
	void FreeJob(EJobType Job);

	void AddBuilding(EBuildingType BuildingType);
	void RemoveBuilding(EBuildingType BuildingType);

	void AddModifier(EDistrictModifier Modifier);
	void RemoveModifier(EDistrictModifier Modifier);

	const TArray<EBuildingType>& GetBuildings() const;
	const TArray<EDistrictModifier>& GetModifiers() const;
	const TMap<EJobType, int32>& GetJobs() const;
	const TMap<EJobType, int32>& GetOccupiedJobs() const;

	bool IsValidBuildingIndex(int32 Index) const;
	int32 GetBuildingCount() const;

	void UpgradeDistrict(EDistrictType Type);
	int32 DowngradeDistrict();

	static EJobType GetDistrictDefaultJob(EDistrictType Type);

	TOptional<EJobType> GetFreeJob() const;

private:
	TMap<EResourceType, float> AddUpProductionModifiers();
	TMap<EResourceType, float> AddUpUpkeepModifiers();

public:
	UPROPERTY(VisibleAnywhere)
	int32 DistrictTier;
	UPROPERTY(VisibleAnywhere)
	EDistrictType DistrictType;

	UPROPERTY(VisibleAnywhere)
	float EnergyProduction;
	UPROPERTY(VisibleAnywhere)
	float EnergyUpkeep;

	UPROPERTY(VisibleAnywhere)
	int32 BuildSlots;

private:
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = true))
	TMap<EJobType, int32> OccupiedJobs;
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = true))
	TMap<EJobType, int32> DistrictJobs;
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = true))
	TArray<EBuildingType> Buildings;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = true))
	TArray<EDistrictModifier> ResourceModifiers;

	TMap<EResourceType, float> ProductionModifiers;
	TMap<EResourceType, float> UpkeepModifiers;
};

USTRUCT()
struct FTradeRoute
{
	GENERATED_BODY()

	AGameActor* Origin;
	AGameActor* Destination;

	const int32* Time;
	int32 Rate;
	float FuelCosts;
	float DangerLevel;
	TMap<EResourceType, float> Resources;

	friend bool operator==(const FTradeRoute& RouteA, const FTradeRoute& RouteB)
	{
		bool bSame = true;
		for (const auto& [Type, Amount] : RouteA.Resources)
		{
			bSame = bSame && (RouteB.Resources.FindRef(Type) == Amount);
		}

		bSame = bSame && RouteA.Origin == RouteB.Origin && RouteA.Destination == RouteB.Destination && RouteA.Rate == RouteB.Rate;

		return bSame;
	}
};

USTRUCT()
struct FBuildSlot
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere)
	FDistrict District;
	UPROPERTY(VisibleAnywhere)
	TArray<EFeatureType> Features;

	FBuildSlot()
		:District(FDistrict()),
		Features({})
	{}
};
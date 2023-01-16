// Fill out your copyright notice in the Description page of Project Settings.


#include "Planet.h"
#include "Camera/CameraComponent.h"
#include "Stardust/GameModes/GameFramework.h"
#include "Stardust/Player/PlayerCorporation.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Stardust/UI/Planet/PlanetWidget.h"



APlanet::APlanet()
{
	WidgetCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	WidgetCamera->SetupAttachment(RootComponent);

	TerrestrialMaterial = CreateDefaultSubobject<UMaterialInterface>(TEXT("Terrestrial Material"));
	OceanMaterial = CreateDefaultSubobject<UMaterialInterface>(TEXT("Ocean Material"));
	BarrenMaterial = CreateDefaultSubobject<UMaterialInterface>(TEXT("Barren Material"));
}

void APlanet::BeginPlay()
{
	Super::BeginPlay();

	CreatePlanet();
}

void APlanet::OnDayUpdate()
{
	Super::OnDayUpdate();

	if (BuildQueue.Num())
		HandleBuilding();

	PartialPopulation += PopulationGrowth / 30;
	if (PartialPopulation >= PopulationGrowthThreshold)
	{
		Population++;
		PartialPopulation -= PopulationGrowthThreshold;

		UnemployedPopulation++;
		OccupyJobs();
	}
}

void APlanet::OnMonthUpdate()
{
	Super::OnMonthUpdate();

	RecalculateProduction();
}

void APlanet::OnClicked()
{
	Super::OnClicked();

	if (!PlanetWidgetClass) return;

	TArray<UUserWidget*> Widgets;
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), Widgets, UPlanetWidget::StaticClass(), false);

	if (Widgets.IsValidIndex(0))
	{
		if (!Widgets[0]->IsInViewport())
		{
			UPlanetWidget* PlanetWidget = CreateWidget<UPlanetWidget>(GetWorld(), PlanetWidgetClass);
			PlanetWidget->AddToViewport();
			PlanetWidget->PreloadData(this);
		}
	}
	else
	{
		UPlanetWidget* PlanetWidget = CreateWidget<UPlanetWidget>(GetWorld(), PlanetWidgetClass);
		PlanetWidget->AddToViewport();
		PlanetWidget->PreloadData(this);
	}

	UE_LOG(LogTemp, Warning, TEXT("[%s][%i]: OnClicked"), *GetName(), __LINE__);
}



void APlanet::CreatePlanet()
{
	constexpr int32 MaxFeaturesPerBuildSlot = 4;

	//Object declarations
	AGameFramework* GameMode = Cast<AGameFramework>(GetWorld()->GetAuthGameMode());
	UEnum* FeatureTypeEnum = FindObject<UEnum>(ANY_PACKAGE, TEXT("EFeatureType"), true);


	if (!FeatureTypeEnum)
		return;

	if (!GameMode)
		return;

	if (!TerrestrialMaterial || !OceanMaterial || !BarrenMaterial)
		return;


	//Sets random planet type
	if (UEnum* PlanetTypeEnum = FindObject<UEnum>(ANY_PACKAGE, TEXT("EPlanetType"), true))
	{
		PlanetType = EPlanetType(FMath::RandRange(0, (int32)PlanetTypeEnum->GetMaxEnumValue() - 1));

		switch (PlanetType)
		{
		case EPlanetType::Ocean:
			ActorMeshComponent->SetMaterial(0, OceanMaterial);
			break;

		case EPlanetType::Barren:
			ActorMeshComponent->SetMaterial(0, BarrenMaterial);
			break;

		case EPlanetType::Terrestrial:
			ActorMeshComponent->SetMaterial(0, TerrestrialMaterial);
			break;
		}
	}
	else PlanetType = EPlanetType::Ocean;

	//Sets planet size range
	FVector2D PlanetSizeRange;
	if (UStructDataLibrary::GetData(PlanetType))
	{
		PlanetSizeRange = UStructDataLibrary::GetData(PlanetType)->Range;
	}
	else
	{
		PlanetSizeRange = FVector2D(5, 8);
	}
	PlanetSize = FMath::RandRange((int32)PlanetSizeRange.X, (int32)PlanetSizeRange.Y);

	//Creates build slots
	BuildSlots.Reserve(PlanetSize);
	for (int32 i = 0; i < PlanetSize; i++)
		BuildSlots.Add(FBuildSlot());

	//Generates features
	for (int32 i = 0; i < FeatureTypeEnum->GetMaxEnumValue(); i++)
	{
		if (UStructDataLibrary::GetData((EFeatureType)i)->bPlanetary)
		{
			GeneratePlanetaryFeature(i);
			continue;
		}

		for (FBuildSlot& BuildSlot : BuildSlots)
			GenerateFeature(i, BuildSlot);
	}

	RecalculateModifiers();

	//Logging
	UE_LOG(LogTemp, Warning, TEXT("--------------------"))
	UE_LOG(LogTemp, Warning, TEXT("PlanetType: [%s]"), *UEnum::GetValueAsString(PlanetType))
	UE_LOG(LogTemp, Warning, TEXT("PlanetSize: [%i]"), PlanetSize)
	UE_LOG(LogTemp, Warning, TEXT("--------------------"))
}

void APlanet::GenerateFeature(int32 FeatureIndex, FBuildSlot& BuildSlot)
{
	const FFeatureData* Feature = UStructDataLibrary::GetData((EFeatureType)FeatureIndex);

	if (!Feature)
		return;

	if (!Feature->RequiredType.Contains(PlanetType) && Feature->RequiredType.Num() > 0)
		return;

	if ((Feature->RequiredSize.X > PlanetSize || PlanetSize > Feature->RequiredSize.Y) && (Feature->RequiredSize.Y - Feature->RequiredSize.X > 0))
		return;

	bool bPrerequisites = true;
	for (EFeatureType PrerequisiteType : Feature->Prerequisites)
		bPrerequisites = (bool)(bPrerequisites * BuildSlot.Features.Contains(PrerequisiteType));

	if (!bPrerequisites)
		return;

	float Chance = Feature->Rarity;
	bool bGenerated;

	do
	{
		bGenerated = FMath::FRand() <= Chance;
		Chance = Chance * Chance * Chance;

		if (bGenerated)
		{
			BuildSlot.Features.Add((EFeatureType)FeatureIndex);

			for (const EDistrictModifier& Modifier : Feature->DistrictModifiers)
				BuildSlot.District.AddModifier(Modifier);
		}

	} while (bGenerated && Feature->bMultiple);
}

void APlanet::GeneratePlanetaryFeature(int32 FeatureIndex)
{
	const FFeatureData* Feature = UStructDataLibrary::GetData((EFeatureType)FeatureIndex);

	if (!Feature)
		return;

	if (!Feature->RequiredType.Contains(PlanetType) && Feature->RequiredType.Num() > 0)
		return;

	if ((Feature->RequiredSize.X > PlanetSize || PlanetSize > Feature->RequiredSize.Y) && (Feature->RequiredSize.Y - Feature->RequiredSize.X > 0))
		return;

	float Chance = Feature->Rarity;
	bool bGenerated;

	do
	{
		bGenerated = FMath::FRand() <= Chance;
		Chance = Chance * Chance * Chance;

		if (bGenerated)
		{
			PlanetaryFeatures.Add((EFeatureType)FeatureIndex);
			PlanetaryModifiers.Append(Feature->PlanetModifiers);
		}

	} while (bGenerated && Feature->bMultiple);
}

void APlanet::ColonizePlanet(APlayerCorporation* Corporation)
{
	if (Corporation)
	{
		PlanetCorporation = Corporation;
		PlanetCorporation.GetValue()->AddPlanet(this);

		if (AGameFramework* GameMode = Cast<AGameFramework>(GetWorld()->GetAuthGameMode()))
		{
			MaxBuildStackCount = GameMode->PlanetStartingBuildStackCount;

			Population = GameMode->PlanetStartingPopulation;
			UnemployedPopulation = Population;
			PopulationGrowth = GameMode->PlanetStartingPopulationGrowth;
			PopulationGrowthThreshold = GameMode->PopulationGrowthThreshold;
		}
	}
}




const UBuildRequest* APlanet::BuildBuilding(int32 BuildSlotIndex, EBuildingType BuildingType)
{
	if (!PlanetCorporation.IsSet()) return nullptr;
	if (!BuildSlots.IsValidIndex(BuildSlotIndex)) return nullptr;

	const FBuildingData* Data = UStructDataLibrary::GetData(BuildingType);
	APlayerCorporation* PlayerCorp = Cast<APlayerCorporation>(PlanetCorporation.GetValue());

	if (!Data) return nullptr;
	if (!PlayerCorp) return nullptr;
	if (!Data->RequiredDistrict.Contains(BuildSlots[BuildSlotIndex].District.DistrictType) && Data->RequiredDistrict.Num() != 0) return nullptr;
	if (!PlayerCorp->Purchase(Data->BuildCost)) return nullptr;


	UBuildRequest* Request = NewObject<UBuildRequest>(this);
	Request->BuildTime = Data->BuildTime;
	Request->BuildSlotIndex = BuildSlotIndex;
	Request->Status = EBuildingStatus::InQueue;

	BuildQueue.Add(Request);

	return Request;
}

const UBuildRequest* APlanet::BuildDistrict(int32 BuildSlotIndex, EDistrictType DistrictType)
{
	if (!PlanetCorporation.IsSet()) return nullptr;
	if (!BuildSlots.IsValidIndex(BuildSlotIndex)) return nullptr;

	const FDistrictData* Data = UStructDataLibrary::GetData(DistrictType, 1);
	APlayerCorporation* PlayerCorp = Cast<APlayerCorporation>(PlanetCorporation.GetValue());

	if (!Data) return nullptr;
	if (!PlayerCorp) return nullptr;
	if (!PlayerCorp->Purchase(Data->BuildCost)) return nullptr;


	UBuildRequest* Request = NewObject<UBuildRequest>(this);
	Request->BuildTime = Data->BuildTime;
	Request->BuildSlotIndex = BuildSlotIndex;
	Request->Status = EBuildingStatus::InQueue;

	BuildQueue.Add(Request);

	return Request;
}

const UBuildRequest* APlanet::UpgradeDistrict(int32 BuildSlotIndex)
{
	if (!PlanetCorporation.IsSet()) return nullptr;
	if (!BuildSlots.IsValidIndex(BuildSlotIndex)) return nullptr;

	const FDistrictData* Data = UStructDataLibrary::GetData(BuildSlots[BuildSlotIndex].District.DistrictType, BuildSlots[BuildSlotIndex].District.DistrictTier + 1);
	APlayerCorporation* PlayerCorp = Cast<APlayerCorporation>(PlanetCorporation.GetValue());

	if (!Data) return nullptr;
	if (!PlayerCorp) return nullptr;
	if (!PlayerCorp->Purchase(Data->BuildCost)) return nullptr;


	UBuildRequest* Request = NewObject<UBuildRequest>(this);
	Request->BuildTime = Data->BuildTime;
	Request->BuildSlotIndex = BuildSlotIndex;
	Request->Status = EBuildingStatus::InQueue;

	BuildQueue.Add(Request);

	return Request;
}

void APlanet::HandleBuilding()
{
	int32 BuildStackCount = (MaxBuildStackCount < BuildQueue.Num()) ? MaxBuildStackCount : BuildQueue.Num();
	for (int32 i = 0; i < BuildStackCount; i++)
	{
		UBuildRequest* const Request = BuildQueue[i];

		Request->BuildTime--;
		if (Request->BuildTime > 0) continue;


		if (Request->Type.IsType<EDistrictType>())
		{
			EDistrictType Type = Request->Type.Get<EDistrictType>();

			BuildSlots[Request->BuildSlotIndex].District.UpgradeDistrict(Type);
		}
		else
		{
			EBuildingType Type = Request->Type.Get<EBuildingType>();

			BuildSlots[Request->BuildSlotIndex].District.AddBuilding(Type);
		}

		UpdateWidgets(Request->BuildSlotIndex);
		OccupyJobs();

		Request->Status = EBuildingStatus::Completed;
		BuildQueue.RemoveAt(i, 1, true);
	}
}

void APlanet::RecalculateModifiers()
{
	ProductionModifiers.Empty();
	UpkeepModifiers.Empty();

	for (const EPlanetModifier& Modifier : PlanetaryModifiers)
	{
		for (const TPair<EResourceType, float>& ResourceModifier : UStructDataLibrary::GetData(Modifier)->ProductionModifiers)
			ProductionModifiers.FindOrAdd(ResourceModifier.Key) += ResourceModifier.Value;

		for (const TPair<EResourceType, float>& ResourceModifier : UStructDataLibrary::GetData(Modifier)->UpkeepModifiers)
			UpkeepModifiers.FindOrAdd(ResourceModifier.Key) += ResourceModifier.Value;
	}
}

void APlanet::RecalculateProduction()
{
	ResourceProduction.Empty();
	ResourceUpkeep.Empty();

	EnergyProduction = 0;
	EnergyConsumption = 0;

	for (const auto& [District, Features] : BuildSlots)
	{
		EnergyProduction += District.EnergyProduction;
		EnergyConsumption += District.EnergyUpkeep;

		for (const TPair<EResourceType, float>& Production : District.GetDistrictProduction())
		{
			ResourceProduction.FindOrAdd(Production.Key) += Production.Value * (ProductionModifiers.Contains(Production.Key) ? ProductionModifiers[Production.Key] : 1);
			ResourceStorage.FindOrAdd(Production.Key) += Production.Value * (ProductionModifiers.Contains(Production.Key) ? ProductionModifiers[Production.Key] : 1);
		}

		for (const TPair<EResourceType, float>& Upkeep : District.GetDistrictUpkeep())
		{
			ResourceUpkeep.FindOrAdd(Upkeep.Key) += Upkeep.Value * (UpkeepModifiers.Contains(Upkeep.Key) ? UpkeepModifiers[Upkeep.Key] : 1);
			ResourceStorage.FindOrAdd(Upkeep.Key) -= Upkeep.Value * (UpkeepModifiers.Contains(Upkeep.Key) ? UpkeepModifiers[Upkeep.Key] : 1);
		}
	}

	EnergyFinal = EnergyProduction - EnergyConsumption;
}

void APlanet::OccupyJobs()
{
	for (int32 i = 0; i < UnemployedPopulation; i++)
	{
		bool bJobFound = false;
		int32 Index = 0;

		do
		{
			if (BuildSlots[Index].District.GetFreeJob())
			{
				BuildSlots[Index].District.PopulateJob(*BuildSlots[Index].District.GetFreeJob());
				bJobFound = true;
				UnemployedPopulation--;
			}
			else
			{
				Index++;
			}

		} while (!bJobFound && Index < BuildSlots.Num());
	}
}

void APlanet::UpdateWidgets(int32 BuildSlotIndex)
{
	TArray<UUserWidget*> Widgets;
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), Widgets, UPlanetWidget::StaticClass(), false);

	if (!Widgets.Num()) return;

	if (UPlanetWidget* PlanetWidget = Cast<UPlanetWidget>(Widgets[0]))
		PlanetWidget->BuildingUpdate(BuildSlotIndex);
}



const FBuildSlot& APlanet::GetBuildSlot(int32 Index)
{
	checkf(BuildSlots.IsValidIndex(Index), TEXT("GetDistrict function called with an invalid index"))
		return BuildSlots[Index];
}

bool APlanet::IsValidBuildSlotIndex(int32 BuildSlotIndex)
{
	return BuildSlots.IsValidIndex(BuildSlotIndex);
}

const TMap<EResourceType, float>& APlanet::GetResourceStorage()
{
	return ResourceStorage;
}

const TMap<EResourceType, float>& APlanet::GetResourceProduction()
{
	return ResourceProduction;
}

const TMap<EResourceType, float>& APlanet::GetResourceUpkeep()
{
	return ResourceUpkeep;
}

TMap<EJobType, int32> APlanet::GetJobs()
{
	TMap<EJobType, int32> Jobs;

	for (const auto& [District, Features] : BuildSlots)
		for (const TPair<EJobType, int32>& DistrictJobs : District.GetJobs())
			Jobs.FindOrAdd(DistrictJobs.Key) += DistrictJobs.Value;

	return Jobs;
}

TMap<EJobType, int32> APlanet::GetOccupiedJobs()
{
	TMap<EJobType, int32> Jobs;

	for (const auto& [District, Features] : BuildSlots)
		for (const TPair<EJobType, int32>& DistrictJobs : District.GetOccupiedJobs())
			Jobs.FindOrAdd(DistrictJobs.Key) += DistrictJobs.Value;

	return Jobs;
}
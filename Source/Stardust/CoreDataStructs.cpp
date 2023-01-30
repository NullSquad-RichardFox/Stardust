// Fill out your copyright notice in the Description page of Project Settings.


#include "CoreDataStructs.h"



UStructDataLibrary::UStructDataLibrary()
{
	DistrictDataTable = LoadDataTable("DataTable'/Game/DataTables/DT_DistrictData.DT_DistrictData'");
	BuildingDataTable = LoadDataTable("DataTable'/Game/DataTables/DT_BuildingData.DT_BuildingData'");
	JobDataTable = LoadDataTable("DataTable'/Game/DataTables/DT_JobData.DT_JobData'");
	DistrictModifiersDataTable = LoadDataTable("DataTable'/Game/DataTables/DT_DistrictModifierData.DT_DistrictModifierData'");
	PlanetModifiersDataTable = LoadDataTable("DataTable'/Game/DataTables/DT_PlanetModifierData.DT_PlanetModifierData'");										
	FeatureDataTable = LoadDataTable("DataTable'/Game/DataTables/DT_FeaturesData.DT_FeaturesData'");
	PlanetSizeRangeDataTable = LoadDataTable("DataTable'/Game/DataTables/DT_PlanetSizeRangeData.DT_PlanetSizeRangeData'");
}

UDataTable* UStructDataLibrary::LoadDataTable(const FName& FilePath)
{
	if (UDataTable* DT = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), NULL, *FilePath.ToString())))
	{
		UE_LOG(LogTemp, Warning, TEXT("DT cast"))
		return DT;
	}

	return nullptr;
}

const FPlanetSizeRangeData* UStructDataLibrary::GetData(EPlanetType PlanetType)
{
	if (!PlanetSizeRangeDataTable) return nullptr;

	UEnum* Enum = FindObject<UEnum>(ANY_PACKAGE, TEXT("EPlanetType"), true);

	if (!Enum) return nullptr;


	FString ContextString("UStructDataLibrary");
	return PlanetSizeRangeDataTable->FindRow<FPlanetSizeRangeData>(*Enum->GetNameStringByValue((int32)PlanetType), ContextString);
}

const FBuildingData* UStructDataLibrary::GetData(EBuildingType BuildingType)
{
	if (!BuildingDataTable) return nullptr;

	if (UEnum* Enum = FindObject<UEnum>(ANY_PACKAGE, TEXT("EBuildingType"), true))
	{
		FString ContextString("UStructDataLibrary");
		return BuildingDataTable->FindRow<FBuildingData>(*Enum->GetNameStringByValue((int32)BuildingType), ContextString);

	}

	return nullptr;
}

const FDistrictData* UStructDataLibrary::GetData(EDistrictType DistrictType, int32 DistrictTier)
{
	if (!DistrictDataTable) return nullptr;

	if (UEnum* Enum = FindObject<UEnum>(ANY_PACKAGE, TEXT("EDistrictType"), true))
	{
		FString ContextString("UStructDataLibrary");
		return DistrictDataTable->FindRow<FDistrictData>(*(Enum->GetNameStringByIndex((int32)DistrictType) + "_" + FString::FromInt(DistrictTier)), ContextString);
	}

	return nullptr;
}

const FJobData* UStructDataLibrary::GetData(EJobType JobType)
{
	if (!JobDataTable) return nullptr;

	if (UEnum* Enum = FindObject<UEnum>(ANY_PACKAGE, TEXT("EJobType"), true))
	{
		FString ContextString("UStructDataLibrary");
		return JobDataTable->FindRow<FJobData>(*Enum->GetNameStringByIndex((int32)JobType), ContextString);
	}

	return nullptr;
}

const FModifierData* UStructDataLibrary::GetData(EDistrictModifier ModifierType)
{
	if (!DistrictModifiersDataTable) return nullptr;

	if (UEnum* Enum = FindObject<UEnum>(ANY_PACKAGE, TEXT("EDistrictModifier"), true))
	{
		FString ContextString("UStructDataLibrary");
		return DistrictModifiersDataTable->FindRow<FModifierData>(*Enum->GetNameStringByIndex((int32)ModifierType), ContextString);
	}

	return nullptr;
}

const FModifierData* UStructDataLibrary::GetData(EPlanetModifier ModifierType)
{
	if (!PlanetModifiersDataTable) return nullptr;

	if (UEnum* Enum = FindObject<UEnum>(ANY_PACKAGE, TEXT("EPlanetModifier"), true))
	{
		FString ContextString("UStructDataLibrary");
		return PlanetModifiersDataTable->FindRow<FModifierData>(*Enum->GetNameStringByIndex((int32)ModifierType), ContextString);
	}

	return nullptr;
}

const FFeatureData* UStructDataLibrary::GetData(EFeatureType FeatureType)
{
	if (!FeatureDataTable)
	{
		return nullptr;
		UE_LOG(LogTemp, Error, TEXT("DataTableNotFound"))
	}
	if (UEnum* Enum = FindObject<UEnum>(ANY_PACKAGE, TEXT("EFeatureType"), true))
	{
		FString ContextString("UStructDataLibrary");
		return FeatureDataTable->FindRow<FFeatureData>(*Enum->GetNameStringByIndex((int32)FeatureType), ContextString);
	}

	return nullptr;
}




TMap<EResourceType, float> FDistrict::GetDistrictProduction() const
{
	TMap<EResourceType, float> JobProduction;

	for (const TPair<EJobType, int32>& Job : OccupiedJobs)
		for (const TPair<EResourceType, float>& Resource : UStructDataLibrary::GetData(Job.Key)->ResourceProduction)
			JobProduction.FindOrAdd(Resource.Key) += Resource.Value * Job.Value;

	for (const TPair<EResourceType, float>& Modifier : ProductionModifiers)
		if (JobProduction.Contains(Modifier.Key))
			JobProduction[Modifier.Key] *= Modifier.Value;

	return JobProduction;
}

TMap<EResourceType, float> FDistrict::GetDistrictUpkeep() const
{
	TMap<EResourceType, float> JobUpkeep;

	for (const EBuildingType& Building : Buildings)
		for (const TPair<EResourceType, float>& BuildingUpkeep : UStructDataLibrary::GetData(Building)->ResourceUpkeep)
			JobUpkeep.FindOrAdd(BuildingUpkeep.Key) += BuildingUpkeep.Value;

	for (const TPair<EJobType, int32>& Job : OccupiedJobs)
		for (const TPair<EResourceType, float>& Resource : UStructDataLibrary::GetData(Job.Key)->ResourceUpkeep)
			JobUpkeep.FindOrAdd(Resource.Key) += Resource.Value * Job.Value;

	for (const TPair<EResourceType, float>& Modifier : UpkeepModifiers)
		if (JobUpkeep.Contains(Modifier.Key))
			JobUpkeep[Modifier.Key] *= Modifier.Value;

	return JobUpkeep;
}

TMap<EResourceType, float> FDistrict::AddUpProductionModifiers()
{
	TMap<EResourceType, float> Modifiers;

	for (const EDistrictModifier& Modifier : ResourceModifiers)
		for (const TPair<EResourceType, float>& ResourceModifier : UStructDataLibrary::GetData(Modifier)->ProductionModifiers)
			Modifiers.FindOrAdd(ResourceModifier.Key) += ResourceModifier.Value;

	return Modifiers;
}

TMap<EResourceType, float> FDistrict::AddUpUpkeepModifiers()
{
	TMap<EResourceType, float> Modifiers;

	for (const EDistrictModifier& Modifier : ResourceModifiers)
		for (const TPair<EResourceType, float>& ResourceModifier : UStructDataLibrary::GetData(Modifier)->UpkeepModifiers)
			Modifiers.FindOrAdd(ResourceModifier.Key) += ResourceModifier.Value;

	return Modifiers;
}

EJobType FDistrict::GetDistrictDefaultJob(EDistrictType Type)
{
	switch (Type)
	{
	case EDistrictType::OreCollection:
		return EJobType::OreMiner;

	case EDistrictType::CarbonCollection:
		return EJobType::CarbonMiner;

	case EDistrictType::SandCollection:
		return EJobType::SandMiner;

	case EDistrictType::WaterCollection:
		return EJobType::WaterMiner;

	case EDistrictType::FoodCollection:
		return EJobType::Farmer;

	case EDistrictType::MetalsManufacture:
		return EJobType::Forger;

	case EDistrictType::GlassManufacture:
		return EJobType::GlassWorker;

	case EDistrictType::SiliconManufacture:
		return EJobType::SiliconWorker;

	case EDistrictType::ElectronicsManufacture:
		return EJobType::ElectronicsWorker;

	case EDistrictType::PowerProduction:
		return EJobType::PhysicsEngineer;

	default:
		return EJobType::MaintenanceWorker;
	}
}

void FDistrict::PopulateJob(EJobType Job)
{
	if (OccupiedJobs.FindRef(Job) < DistrictJobs.FindRef(Job))
		OccupiedJobs.FindOrAdd(Job)++;

}

void FDistrict::FreeJob(EJobType Job)
{
	if (OccupiedJobs.Contains(Job))
		OccupiedJobs[Job] = FMath::Clamp(OccupiedJobs[Job] - 1, 0, 10000);
}

void FDistrict::AddBuilding(EBuildingType BuildingType)
{
	Buildings.Add(BuildingType);

	if (const FBuildingData* BuildingData = UStructDataLibrary::GetData(BuildingType))
	{
		for (const TPair<EJobType, int32>& Job : BuildingData->BuildingJobs)
			DistrictJobs.FindOrAdd(Job.Key) += Job.Value;

		EnergyProduction += BuildingData->EnergyProduction;
		EnergyUpkeep += BuildingData->EnergyUpkeep;
	}
}

void FDistrict::RemoveBuilding(EBuildingType BuildingType)
{
	Buildings.Remove(BuildingType);

	if (const FBuildingData* BuildingData = UStructDataLibrary::GetData(BuildingType))
	{
		for (const TPair<EJobType, int32>& Job : BuildingData->BuildingJobs)
			DistrictJobs.FindOrAdd(Job.Key) -= Job.Value;

		EnergyProduction -= BuildingData->EnergyProduction;
		EnergyUpkeep -= BuildingData->EnergyUpkeep;
	}
}

void FDistrict::AddModifier(EDistrictModifier Modifier)
{
	ResourceModifiers.Emplace(Modifier);
	ProductionModifiers = AddUpProductionModifiers();
	UpkeepModifiers = AddUpUpkeepModifiers();
}

void FDistrict::RemoveModifier(EDistrictModifier Modifier)
{
	ResourceModifiers.Remove(Modifier);
	ProductionModifiers = AddUpProductionModifiers();
	UpkeepModifiers = AddUpUpkeepModifiers();
}

const TArray<EBuildingType>& FDistrict::GetBuildings() const
{
	return Buildings;
}

const TArray<EDistrictModifier>& FDistrict::GetModifiers() const
{
	return ResourceModifiers;
}

const TMap<EJobType, int32>& FDistrict::GetJobs() const
{
	return DistrictJobs;
}

const TMap<EJobType, int32>& FDistrict::GetOccupiedJobs() const
{
	return OccupiedJobs;
}

bool FDistrict::IsValidBuildingIndex(int32 Index) const
{
	return Buildings.IsValidIndex(Index);
}

int32 FDistrict::GetBuildingCount() const
{
	return Buildings.Num();
}

void FDistrict::UpgradeDistrict(EDistrictType Type)
{
	DistrictTier++;
	DistrictType = Type;

	if (const FDistrictData* DistrictData = UStructDataLibrary::GetData(DistrictType, DistrictTier))
	{
		EnergyProduction += DistrictData->EnergyProduction;
		EnergyUpkeep += DistrictData->EnergyUpkeep;
		BuildSlots += DistrictData->BuildSlots;

		for (const TPair<EJobType, int32>& Job : DistrictData->DistrictJobs)
		{
			if (Job.Key == EJobType::DistrictDefault)
			{
				DistrictJobs.FindOrAdd(FDistrict::GetDistrictDefaultJob(DistrictType)) += Job.Value;
			}
			else
			{
				DistrictJobs.FindOrAdd(Job.Key) += Job.Value;
			}
		}
	}
}

int32 FDistrict::DowngradeDistrict()
{
	if (DistrictTier == 0) return 0;

	if (const FDistrictData* DistrictData = UStructDataLibrary::GetData(DistrictType, DistrictTier))
	{
		EnergyProduction -= DistrictData->EnergyProduction;
		EnergyUpkeep -= DistrictData->EnergyUpkeep;
		BuildSlots -= DistrictData->BuildSlots;

		for (const TPair<EJobType, int32>& Job : DistrictData->DistrictJobs)
		{
			if (Job.Key == EJobType::DistrictDefault)
			{
				DistrictJobs.FindOrAdd(FDistrict::GetDistrictDefaultJob(DistrictType)) -= Job.Value;
			}
			else
			{
				DistrictJobs.FindOrAdd(Job.Key) -= Job.Value;
			}
		}
	}

	DistrictTier--;
	return DistrictTier;
}

TOptional<EJobType> FDistrict::GetFreeJob() const
{
	for (const TPair<EJobType, int32>& Job : DistrictJobs)
	{
		if (OccupiedJobs.Contains(Job.Key))
		{
			if (OccupiedJobs[Job.Key] < DistrictJobs[Job.Key])
			{
				return Job.Key;
			}
			else
			{
				return {};
			}
		}
		else
		{
			return Job.Key;
		}
	}

	return {};
}
// Fill out your copyright notice in the Description page of Project Settings.


#include "DistrictMenu.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/ListView.h"
#include "Components/TileView.h"
#include "Components/CanvasPanelSlot.h"
#include "Stardust/UI/Planet/BuildingSlotWidget.h"
#include "Stardust/UI/Planet/PlanetWidget.h"

#include "Stardust/GameObjects/Planet.h"
#include "Stardust/GameModes/GameFramework.h"

#include "Stardust/Libraries/ListDataLibrary.h"
#include "Stardust/Libraries/InputFunctionLibrary.h"



void UDistrictMenu::NativeOnInitialized()
{
	if (AGameFramework* GameMode = Cast<AGameFramework>(GetWorld()->GetAuthGameMode()))
	{
		GameMode->MonthUpdateEvent.AddDynamic(this, &UDistrictMenu::MonthlyUpdate);
		FeatureInfoHitBox->OnHovered.AddDynamic(this, &UDistrictMenu::FeatureInfoHovered);
		FeatureInfoHitBox->OnUnhovered.AddDynamic(this, &UDistrictMenu::FeatureInfoUnhovered);

		UpgradeHitBox->OnClicked.AddDynamic(this, &UDistrictMenu::UpgradeDistrict);
		RemoveHitBox->OnClicked.AddDynamic(this, &UDistrictMenu::DowngradeDistrict);
	}
}

void UDistrictMenu::RemoveFromParent()
{
	Super::RemoveFromParent();

	UInputFunctionLibrary::RemoveInputMapping(WidgetMappingContext);
	RemoveBuildingSlots();
}

void UDistrictMenu::MonthlyUpdate()
{
	APlanet* OwningPlanet = Cast<APlanet>(OwningActor);
	if (!OwningPlanet) return;
	if (CurrentBuildSlotIndex == -1) return;

	const FDistrict& District = OwningPlanet->GetBuildSlot(CurrentBuildSlotIndex).District;

	PopulateResourceList(District);
	PopulateJobList(District);
}





void UDistrictMenu::PreloadData(AActor* ParentActor)
{
	if (!ParentActor) return;

	OwningActor = ParentActor;
	CurrentBuildSlotIndex = -1;
	ListBuildingShownIndex = -1;
	bCanBuildBuildings = true;
}

void UDistrictMenu::BuildingUpdate(int32 BuildSlotIndex)
{
	if (BuildSlotIndex != CurrentBuildSlotIndex) return;
	if (!BuildingSlotWidgetClass) return;

	APlanet* OwningPlanet = Cast<APlanet>(OwningActor);
	if (!OwningPlanet) return;



	const FDistrict& District = OwningPlanet->GetBuildSlot(CurrentBuildSlotIndex).District;

	PopulateResourceList(District);
	PopulateJobList(District);

	SetDistrictName(District);

	RemoveBuildingSlots();
	GenerateBuildingSlots(District);
}

bool UDistrictMenu::Reload(int32 BuildSlotIndex)
{
	if (!OwningActor) return false;

	APlanet* OwningPlanet = Cast<APlanet>(OwningActor);

	if (!OwningPlanet) return false;



	if (BuildSlotIndex == CurrentBuildSlotIndex)
	{
		CurrentBuildSlotIndex = -1;
		UInputFunctionLibrary::RemoveInputMapping(WidgetMappingContext);
		return false;
	}


	CurrentBuildSlotIndex = BuildSlotIndex;
	const FDistrict& District = OwningPlanet->GetBuildSlot(CurrentBuildSlotIndex).District;

	BindInput();

	if (District.DistrictTier == 0)
	{
		PopulateDistrictList();
		DisplayDistrictList();
		DisplayDistrictListBlueprint();
	}
	else
	{
		PopulateJobList(District);
		PopulateResourceList(District);

		SetDistrictName(District);

		RemoveBuildingSlots();
		GenerateBuildingSlots(District);

		DisplayNormal();
		DisplayNormalBlueprint();
	}

	return true;
}

void UDistrictMenu::FeatureInfoHovered()
{
	if (CurrentBuildSlotIndex == -1) return;
	if (!OwningActor) return;

	APlanet* OwningPlanet = Cast<APlanet>(OwningActor);
	UEnum* FeatureTypeEnum = FindObject<UEnum>(ANY_PACKAGE, TEXT("EFeatureType"), true);

	if (!OwningPlanet || !FeatureTypeEnum) return;



	const TArray<EFeatureType>& Features = OwningPlanet->GetBuildSlot(CurrentBuildSlotIndex).Features;
	FString FeatureText = "Feature: ";

	for (const EFeatureType& Feature : Features)
	{
		if (Feature != EFeatureType::Default)
			FeatureText += (FeatureTypeEnum->GetDisplayNameTextByIndex((int32)Feature).ToString() + ", ");
	}

	FeatureInfoText->SetText(FText::FromString(FeatureText));
	FeatureInfoText->SetVisibility(ESlateVisibility::Visible);
}

void UDistrictMenu::FeatureInfoUnhovered()
{
	FeatureInfoText->SetVisibility(ESlateVisibility::Collapsed);
}

void UDistrictMenu::UpgradeDistrict()
{
	if (APlanet* OwningPlanet = Cast<APlanet>(OwningActor))
	{
		OwningPlanet->UpgradeDistrict(CurrentBuildSlotIndex);
	}
}

void UDistrictMenu::DowngradeDistrict()
{
	if (APlanet* OwningPlanet = Cast<APlanet>(OwningActor))
	{
		OwningPlanet->DowngradeDistrict(CurrentBuildSlotIndex);
	}
}

void UDistrictMenu::CloseWidget()
{
	if (UPlanetWidget* PlanetWidget = Cast<UPlanetWidget>(GetParent()->GetOuter()->GetOuter()))
		PlanetWidget->BuildSlotClicked(CurrentBuildSlotIndex);
}





void UDistrictMenu::SetCanBuildDistricts(bool bCanBuild)
{
	DistrictList->SetIsEnabled(bCanBuild);

	if (!bCanBuild)
	{
		AlreadyBuildingIcon->SetVisibility(ESlateVisibility::Visible);
		AlreadyBuildingBackgroundEffect->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		AlreadyBuildingIcon->SetVisibility(ESlateVisibility::Collapsed);
		AlreadyBuildingBackgroundEffect->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UDistrictMenu::SetCanBuildBuildings(bool bCanBuild)
{
	bCanBuildBuildings = bCanBuild;
	BuildingList->SetIsEnabled(bCanBuild);
}

void UDistrictMenu::BuildingSlotClicked(int32 BuildingSlotIndex)
{
	if (BuildingList->GetVisibility() == ESlateVisibility::Collapsed || ListBuildingShownIndex != BuildingSlotIndex)
	{
		PopulateBuildingList();
		DisplayBuildingList();
		DisplayBuildingListBlueprint();
	}
	else
	{
		ListBuildingShownIndex = -1;

		DisplayNormal();
		DisplayNormalBlueprint();
	}
}




void UDistrictMenu::BindInput()
{
	WidgetMappingContext = NewObject<UInputMappingContext>(this);

	CloseWidgetAction = NewObject<UInputAction>(this);
	CloseWidgetAction->ValueType = EInputActionValueType::Boolean;
	CloseWidgetAction->bConsumeInput = true;

	UInputFunctionLibrary::BindInputAction(this, TEXT("CloseWidget"), ETriggerEvent::Started, WidgetMappingContext, CloseWidgetAction, EKeys::Q);
	UInputFunctionLibrary::AddInputMapping(WidgetMappingContext, 2);
}

void UDistrictMenu::DisplayDistrictList()
{
	AlreadyBuildingIcon->SetVisibility(ESlateVisibility::Collapsed);
	AlreadyBuildingBackgroundEffect->SetVisibility(ESlateVisibility::Collapsed);
	FeatureInfoText->SetVisibility(ESlateVisibility::Collapsed);

	UpgradeHitBox->SetVisibility(ESlateVisibility::Collapsed);
	RemoveHitBox->SetVisibility(ESlateVisibility::Collapsed);
	DistrictName->SetVisibility(ESlateVisibility::Collapsed);
	ResourceFlowList->SetVisibility(ESlateVisibility::Collapsed);
	JobList->SetVisibility(ESlateVisibility::Collapsed);
	BuildingList->SetVisibility(ESlateVisibility::Collapsed);

	DistrictList->SetVisibility(ESlateVisibility::Visible);
}
void UDistrictMenu::DisplayBuildingList()
{
	AlreadyBuildingIcon->SetVisibility(ESlateVisibility::Collapsed);
	AlreadyBuildingBackgroundEffect->SetVisibility(ESlateVisibility::Collapsed);
	FeatureInfoText->SetVisibility(ESlateVisibility::Collapsed);

	JobList->SetVisibility(ESlateVisibility::Collapsed);
	DistrictList->SetVisibility(ESlateVisibility::Collapsed);

	UpgradeHitBox->SetVisibility(ESlateVisibility::Visible);
	RemoveHitBox->SetVisibility(ESlateVisibility::Visible);
	DistrictName->SetVisibility(ESlateVisibility::Visible);
	ResourceFlowList->SetVisibility(ESlateVisibility::Visible);
	BuildingList->SetVisibility(ESlateVisibility::Visible);
}
void UDistrictMenu::DisplayNormal()
{
	AlreadyBuildingIcon->SetVisibility(ESlateVisibility::Collapsed);
	AlreadyBuildingBackgroundEffect->SetVisibility(ESlateVisibility::Collapsed);
	FeatureInfoText->SetVisibility(ESlateVisibility::Collapsed);

	DistrictList->SetVisibility(ESlateVisibility::Collapsed);
	BuildingList->SetVisibility(ESlateVisibility::Collapsed);

	UpgradeHitBox->SetVisibility(ESlateVisibility::Visible);
	RemoveHitBox->SetVisibility(ESlateVisibility::Visible);
	DistrictName->SetVisibility(ESlateVisibility::Visible);
	ResourceFlowList->SetVisibility(ESlateVisibility::Visible);
	JobList->SetVisibility(ESlateVisibility::Visible);
}

void UDistrictMenu::PopulateJobList(const FDistrict& District)
{
	JobList->ClearListItems();

	const TMap<EJobType, int32> OccupiedJobs = District.GetOccupiedJobs();

	//Updates job list
	for (const auto& [JobType, JobCount] : District.GetJobs())
	{
		OccupiedJobs.Contains(JobType);

		UJobListData* Item = NewObject<UJobListData>(this);
		Item->MakeJobData(JobType, JobCount, OccupiedJobs.Contains(JobType) ? OccupiedJobs[JobType] : 0);
		JobList->AddItem(Item);
	}
}
void UDistrictMenu::PopulateResourceList(const FDistrict& District)
{
	ResourceFlowList->ClearListItems();

	TMap<EResourceType, float> BuildSlotResources = District.GetDistrictProduction();

	//Sums the resource generation 
	for (const auto& [ResourceType, ResourceAmount] : District.GetDistrictUpkeep())
		BuildSlotResources.FindOrAdd(ResourceType) -= ResourceAmount;

	//Updates planet list
	for (const auto& [ResourceType, ResourceAmount] : BuildSlotResources)
	{
		UResourceFlowListData* Item = NewObject<UResourceFlowListData>(this);
		Item->MakeResourceFlowData(ResourceAmount, ResourceType);
		ResourceFlowList->AddItem(Item);
	}
}
void UDistrictMenu::PopulateDistrictList()
{
	DistrictList->ClearListItems();

	//Updates district list
	if (UEnum* DistrictTypeEnum = FindObject<UEnum>(ANY_PACKAGE, TEXT("EDistrictType"), true))
	{
		for (int32 i = 0; i < DistrictTypeEnum->NumEnums() - 1; i++)
		{
			UDistrictListData* Item = NewObject<UDistrictListData>(this);
			Item->MakeDistrictData(CurrentBuildSlotIndex, (EDistrictType)i, OwningActor);
			DistrictList->AddItem(Item);
		}
	}
}
void UDistrictMenu::PopulateBuildingList()
{
	BuildingList->ClearListItems();

	//Updates district list
	if (UEnum* BuildingTypeEnum = FindObject<UEnum>(ANY_PACKAGE, TEXT("EBuildingType"), true))
	{
		for (int32 i = 0; i < BuildingTypeEnum->NumEnums() - 1; i++)
		{
			UBuildingListData* Item = NewObject<UBuildingListData>(this);
			Item->MakeBuildingData(CurrentBuildSlotIndex, (EBuildingType)i, OwningActor);
			BuildingList->AddItem(Item);
		}
	}
}

void UDistrictMenu::SetDistrictName(const FDistrict& District)
{
	UEnum* DistrictTypeEnum = FindObject<UEnum>(ANY_PACKAGE, TEXT("EDistrictType"), true);

	if (!DistrictTypeEnum) return;



	FString DistrictNameText =
		DistrictTypeEnum->GetDisplayNameTextByIndex((int32)District.DistrictType).ToString()
		+ " - Tier "
		+ FString::FormatAsNumber(District.DistrictTier);

	DistrictName->SetText(FText::FromString(DistrictNameText));
}

void UDistrictMenu::RemoveBuildingSlots()
{
	UPanelWidget* Panel = Cast<UPanelWidget>(GetRootWidget());
	if (!Panel) return;



	for (UWidget* Child : Panel->GetAllChildren())
	{
		if (UBuildingSlotWidget* BuildingSlot = Cast<UBuildingSlotWidget>(Child))
			BuildingSlot->RemoveFromParent();
	}
}
void UDistrictMenu::GenerateBuildingSlots(const FDistrict& District)
{
	UPanelWidget* Panel = Cast<UPanelWidget>(GetRootWidget());

	if (!Panel) return;



	for (int32 i = 0; i < District.BuildSlots; i++)
	{
		UBuildingSlotWidget* BuildSlot = CreateWidget<UBuildingSlotWidget>(GetWorld(), BuildingSlotWidgetClass);

		Panel->AddChild(BuildSlot);
		if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(BuildSlot->Slot))
		{
			CanvasSlot->SetPosition(FVector2D(1300.f + 125.f * (i % 5), 375.f + 125.f * (i / 5)));
		}
	}
}
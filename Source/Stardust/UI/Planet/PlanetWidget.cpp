// Fill out your copyright notice in the Description page of Project Settings.


#include "PlanetWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/CanvasPanelSlot.h"
#include "Stardust/UI/Components/RadialProgressBar.h"
#include "Stardust/UI/Planet/BuildSlotWidget.h"
#include "Stardust/UI/Planet/PlanetMenu.h"
#include "Stardust/UI/Planet/DistrictMenu.h"

#include "Stardust/Player/SpectatorPlayerController.h"
#include "Stardust/Player/SpectatorPlayerPawn.h"
#include "Stardust/Player/PlayerCorporation.h"
#include "Stardust/GameModes/GameFramework.h"
#include "Stardust/GameObjects/Planet.h"

#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"
#include "Stardust/Libraries/InputFunctionLibrary.h"



void UPlanetWidget::NativeOnInitialized()
{
	if (AGameFramework* GameMode = Cast<AGameFramework>(GetWorld()->GetAuthGameMode()))
	{
		GameMode->DayUpdateEvent.AddDynamic(this, &UPlanetWidget::MonthUpdate);
		ColonizeButton->OnClicked.AddDynamic(this, &UPlanetWidget::ColonizePlanet);
		FeatureInfoHitBox->OnHovered.AddDynamic(this, &UPlanetWidget::FeatureInfoHovered);
		FeatureInfoHitBox->OnUnhovered.AddDynamic(this, &UPlanetWidget::FeatureInfoUnhovered);
	}
}

void UPlanetWidget::PreloadData(APlanet* ParentPlanet)
{
	if (!ParentPlanet || !BuildSlotWidgetClass) return;

	OwningPlanet = ParentPlanet;
	OwningPlanet->BuildingFinishedEvent.AddDynamic(this, &UPlanetWidget::BuildingUpdate);

	EnablePlayerMovement(false);
	BindInput();

	TArray<AActor*> PlayerCorp;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerCorporation::StaticClass(), PlayerCorp);

	if (PlayerCorp.IsValidIndex(0))
	{
		if (APlayerCorporation* PlayerCorporation = Cast< APlayerCorporation>(PlayerCorp[0]))
		{
			if (PlayerCorporation->IsPlanetOwned(OwningPlanet))
			{
				DisplayPlanetData();
				return;
			}
		}
	}

	DisplayColonizeData();
}

void UPlanetWidget::DisplayPlanetData()
{
	EnergyBar->SetAlpha(OwningPlanet->EnergyConsumption / OwningPlanet->EnergyProduction);
	EnergyBar->SetCount(OwningPlanet->EnergyFinal);

	PopulationBar->SetAlpha(OwningPlanet->PopulationGrowth / OwningPlanet->PopulationGrowthThreshold);
	PopulationBar->SetCount(OwningPlanet->Population);

	ColonizeButton->SetVisibility(ESlateVisibility::Collapsed);
	ColonizeText->SetVisibility(ESlateVisibility::Collapsed);
	FeatureInfoText->SetVisibility(ESlateVisibility::Collapsed);

	EnergyBar->SetVisibility(ESlateVisibility::Visible);
	PopulationBar->SetVisibility(ESlateVisibility::Visible);
	StorageBar->SetVisibility(ESlateVisibility::Visible);
	PlanetMenu->SetVisibility(ESlateVisibility::Visible);

	DistrictMenu->SetVisibility(ESlateVisibility::Collapsed);

	DistrictMenu->PreloadData(OwningPlanet);
	PlanetMenu->Preload(OwningPlanet);

	//Creates & initializes build slots	
	if (UPanelWidget* Panel = Cast<UPanelWidget>(GetRootWidget()))
	{
		for (int32 i = 0; i < OwningPlanet->PlanetSize; i++)
		{
			UBuildSlotWidget* BuildSlot = CreateWidget<UBuildSlotWidget>(GetWorld(), BuildSlotWidgetClass);

			Panel->AddChild(BuildSlot);

			BuildSlot->Preload(OwningPlanet, i);
			if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(BuildSlot->Slot))
			{
				CanvasSlot->SetPosition(FVector2D(90.f + 134.f * (i % 5), 762.f + 131.f * (i / 5)));
			}

			BuildSlots.Emplace(BuildSlot);
		}
	}
}

void UPlanetWidget::DisplayColonizeData()
{
	FeatureInfoText->SetVisibility(ESlateVisibility::Collapsed);
	EnergyBar->SetVisibility(ESlateVisibility::Collapsed);
	PopulationBar->SetVisibility(ESlateVisibility::Collapsed);
	StorageBar->SetVisibility(ESlateVisibility::Collapsed);
	PlanetMenu->SetVisibility(ESlateVisibility::Collapsed);
	DistrictMenu->SetVisibility(ESlateVisibility::Collapsed);

	ColonizeButton->SetVisibility(ESlateVisibility::Visible);
	ColonizeText->SetVisibility(ESlateVisibility::Visible);
}

void UPlanetWidget::MonthUpdate()
{
	if (!OwningPlanet) return;

	PopulationBar->SetAlpha(OwningPlanet->PartialPopulation / OwningPlanet->PopulationGrowthThreshold);
	PopulationBar->SetCount(OwningPlanet->Population);
}

void UPlanetWidget::ColonizePlanet()
{
	if (!OwningPlanet) return;

	TArray<AActor*> PlayerCorp;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerCorporation::StaticClass(), PlayerCorp);

	if (PlayerCorp.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("No player corporation"))
			return;
	}
	else if (PlayerCorp.Num() > 1)
	{
		UE_LOG(LogTemp, Error, TEXT("Multiple player corporations"))
			return;
	}

	if (OwningPlanet->ColonizePlanet(Cast<APlayerCorporation>(PlayerCorp[0])))
		DisplayPlanetData();
}

void UPlanetWidget::FeatureInfoHovered()
{
	FString FeatureText = "Feature: ";

	if (UEnum* FeatureTypeEnum = FindObject<UEnum>(ANY_PACKAGE, TEXT("EFeatureType"), true))
	{
		for (const EFeatureType& Feature : OwningPlanet->PlanetaryFeatures)
		{
			if (Feature != EFeatureType::Default)
				FeatureText += (FeatureTypeEnum->GetDisplayNameTextByIndex((int32)Feature).ToString() + ", ");
		}
	}

	FeatureInfoText->SetText(FText::FromString(FeatureText));
	FeatureInfoText->SetVisibility(ESlateVisibility::Visible);
}

void UPlanetWidget::FeatureInfoUnhovered()
{
	FeatureInfoText->SetVisibility(ESlateVisibility::Collapsed);
}

//Updates widget after a building is built
void UPlanetWidget::BuildingUpdate(int32 BuildSlotIndex)
{
	if (!BuildSlots.IsValidIndex(BuildSlotIndex) || !OwningPlanet) return;

	EnergyBar->SetAlpha(OwningPlanet->EnergyConsumption / OwningPlanet->EnergyProduction);
	EnergyBar->SetCount(OwningPlanet->EnergyFinal);
}

void UPlanetWidget::AddQueueItem(UObject* Item)
{
	PlanetMenu->AddQueueItem(Item);
}

void UPlanetWidget::EnablePlayerMovement(bool Value)
{
	ASpectatorPlayerPawn* Player = Cast<ASpectatorPlayerPawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (!Player) return;

	if (Value)
	{
		Player->EnableMovement();
		CameraTransition(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	}
	else
	{
		Player->DisableMovement();
		CameraTransition(OwningPlanet);	
	}
}


void UPlanetWidget::BuildSlotClicked(int32 BuildSlotIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("BuildSlot: %i"), BuildSlotIndex);
	if (DistrictMenu->Reload(BuildSlotIndex))
	{
		PlanetMenu->SetVisibility(ESlateVisibility::Collapsed);
		DistrictMenu->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		PlanetMenu->SetVisibility(ESlateVisibility::Visible);
		DistrictMenu->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UPlanetWidget::RemoveFromParent()
{
	UInputFunctionLibrary::RemoveInputMapping(WidgetMappingContext);

	EnablePlayerMovement(true);

	//Removes build slots
	if (UPanelWidget* Panel = Cast<UPanelWidget>(GetRootWidget()))
	{
		for (UWidget* Child : Panel->GetAllChildren())
			if (UBuildSlotWidget* BuildSlot = Cast<UBuildSlotWidget>(Child))
			{
				BuildSlot->RemoveFromParent();
			}
	}

	Super::RemoveFromParent();
}

void UPlanetWidget::BindInput()
{
	WidgetMappingContext = NewObject<UInputMappingContext>(this);

	CloseWidgetAction = NewObject<UInputAction>(this);
	CloseWidgetAction->ValueType = EInputActionValueType::Boolean;
	CloseWidgetAction->bConsumeInput = true;

	UInputFunctionLibrary::BindInputAction(this, TEXT("RemoveFromParent"), ETriggerEvent::Started, WidgetMappingContext, CloseWidgetAction, EKeys::Q);
	UInputFunctionLibrary::AddInputMapping(WidgetMappingContext, 1);
}

void UPlanetWidget::CameraTransition(AActor* Target)
{
	if (ASpectatorPlayerController* PlayerController = Cast<ASpectatorPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0)))
	{
		if (APlayerCameraManager* CamManager = Cast<APlayerCameraManager>(PlayerController->PlayerCameraManager))
		{
			FViewTargetTransitionParams TransitionParams;
			TransitionParams.BlendTime = 0.25f;
			TransitionParams.BlendFunction = VTBlend_EaseInOut;
			TransitionParams.BlendExp = 1.f;

			CamManager->SetViewTarget(Target, TransitionParams);
		}
	}
}
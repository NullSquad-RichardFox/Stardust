// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlanetWidget.generated.h"


class UImage;
class UButton;
class UTextBlock;
class UPlanetMenu;
class UDistrictMenu;
class UBuildSlotWidget;
class URadialProgressBar;
class APlanet;
class UInputAction;
class UInputMappingContext;

UCLASS()
class STARDUST_API UPlanetWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// call right after AddToViewport
	void PreloadData(APlanet* ParentPlanet);
	void BuildingUpdate(int32 BuildSlotIndex);
	void BuildSlotClicked(int32 BuildSlotIndex);

	void AddQueueItem(UObject* Item);

protected:
	virtual void NativeOnInitialized() override;
	virtual void RemoveFromParent() override;

private:
	UFUNCTION()
	void MonthUpdate();

	UFUNCTION()
	void ColonizePlanet();
	UFUNCTION()
	void FeatureInfoHovered();
	UFUNCTION()
	void FeatureInfoUnhovered();

	void DisplayPlanetData();
	void DisplayColonizeData();

	void CameraTransition(AActor* Target);
	void BindInput();

protected:
	UPROPERTY(EditAnywhere, Category = "Components|Planet", meta = (BindWidget))
	UTextBlock* PlanetName;

	UPROPERTY(EditAnywhere, Category = "Components|SideBars", meta = (BindWidget))
	URadialProgressBar* EnergyBar;
	UPROPERTY(EditAnywhere, Category = "Components|SideBars", meta = (BindWidget))
	URadialProgressBar* PopulationBar;
	UPROPERTY(EditAnywhere, Category = "Components|SideBars", meta = (BindWidget))
	URadialProgressBar* StorageBar;

	UPROPERTY(EditAnywhere, Category = "Components", meta = (BindWidget))
	UButton* FeatureInfoHitBox;
	UPROPERTY(EditAnywhere, Category = "Components", meta = (BindWidget))
	UImage* FeatureInfoIcon;
	UPROPERTY(EditAnywhere, Category = "Components", meta = (BindWidget))
	UTextBlock* FeatureInfoText;

	UPROPERTY(EditAnywhere, Category = "Components|Colonization", meta = (BindWidget))
	UButton* ColonizeButton;
	UPROPERTY(EditAnywhere, Category = "Components|Colonization", meta = (BindWidget))
	UTextBlock* ColonizeText;

	UPROPERTY(EditAnywhere, Category = "Components|Side", meta = (BindWidget))
	UPlanetMenu* PlanetMenu;
	UPROPERTY(EditAnywhere, Category = "Components|Side", meta = (BindWidget))
	UDistrictMenu* DistrictMenu;
	
private:
	UPROPERTY()
	UInputMappingContext* WidgetMappingContext;

	UPROPERTY()
	UInputAction* CloseWidgetAction;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TSubclassOf<UBuildSlotWidget> BuildSlotWidgetClass;

	APlanet* OwningPlanet;
	TArray<UBuildSlotWidget*> BuildSlots;
};

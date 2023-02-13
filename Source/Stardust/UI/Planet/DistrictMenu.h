
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Stardust/CoreDataStructs.h"
#include "DistrictMenu.generated.h"


class UImage;
class UButton;
class UTextBlock;
class UTileView;
class UListView;
class UBuildingSlotWidget;
class UInputAction;
class UInputMappingContext;
class AGameActor;

UCLASS()
class STARDUST_API UDistrictMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	void PreloadData(AGameActor* OwningActor);
	bool Reload(int32 BuildSlotIndex);

	void BuildingSlotClicked(int32 BuildingSlotIndex);

	void SetCanBuildDistricts(bool bCanBuild, int32 DistrictIndex);
	void SetCanBuildBuildings(bool bCanBuild, int32 DistrictIndex);

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void DisplayDistrictListBlueprint();
	UFUNCTION(BlueprintImplementableEvent)
	void DisplayBuildingListBlueprint();
	UFUNCTION(BlueprintImplementableEvent)
	void DisplayNormalBlueprint();

	virtual void NativeOnInitialized() override;
	virtual void RemoveFromParent() override;

private:
	UFUNCTION()
	void MonthlyUpdate();

	UFUNCTION()
	void BuildingUpdate(int32 BuildSlotIndex);
	UFUNCTION()
	void PopulationUpdate();

	UFUNCTION()
	void FeatureInfoHovered();
	UFUNCTION()
	void FeatureInfoUnhovered();
	UFUNCTION()
	void UpgradeDistrict();
	UFUNCTION()
	void DowngradeDistrict();

	UFUNCTION()
	void CloseWidget();

	void DisplayDistrictList();
	void DisplayBuildingList();
	void DisplayNormal();

	void PopulateJobList(const FDistrict& District);
	void PopulateResourceList(const FDistrict& District);
	void PopulateDistrictList();
	void PopulateBuildingList();

	void SetDistrictName(const FDistrict& District);
	void BindInput();
	void RemoveBuildingSlots();
	void GenerateBuildingSlots(const FDistrict& District);

protected:
	UPROPERTY(EditAnywhere, Category = "Components", meta = (BindWidget))
	UButton* UpgradeHitBox;
	UPROPERTY(EditAnywhere, Category = "Components", meta = (BindWidget))
	UButton* RemoveHitBox;
	UPROPERTY(EditAnywhere, Category = "Components", meta = (BindWidget))
	UTextBlock* DistrictName;

	UPROPERTY(EditAnywhere, Category = "Components", meta = (BindWidget))
	UButton* FeatureInfoHitBox;
	UPROPERTY(EditAnywhere, Category = "Components", meta = (BindWidget))
	UImage* FeatureInfoIcon;
	UPROPERTY(EditAnywhere, Category = "Components", meta = (BindWidget))
	UTextBlock* FeatureInfoText;
	UPROPERTY(EditAnywhere, Category = "Components", meta = (BindWidget))
	UImage* AlreadyBuildingIcon;
	UPROPERTY(EditAnywhere, Category = "Components", meta = (BindWidget))
	UImage* AlreadyBuildingBackgroundEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (BindWidget))
	UListView* ResourceFlowList;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (BindWidget))
	UTileView* JobList;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (BindWidget))
	UTileView* DistrictList;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (BindWidget))
	UTileView* BuildingList;

private:
	UPROPERTY()
	UInputMappingContext* WidgetMappingContext;
	UPROPERTY()
	UInputAction* CloseWidgetAction;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TSubclassOf<UBuildingSlotWidget> BuildingSlotWidgetClass;

	AGameActor* OwningActor;
	int32 CurrentBuildSlotIndex;

	int32 LastBuildingIndexShown;

	TArray<int32> LockedDistrictIndexes;
	TArray<int32> LockedBuildingIndexes;
};

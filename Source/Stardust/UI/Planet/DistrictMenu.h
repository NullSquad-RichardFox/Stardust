
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

UCLASS()
class STARDUST_API UDistrictMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	void PreloadData(AActor* OwningActor);
	bool Reload(int32 BuildSlotIndex);
	void BuildingUpdate(int32 BuildSlotIndex);
	void LockDistrictList();

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void DisplayDistrictListBlueprint();
	UFUNCTION(BlueprintImplementableEvent)
	void DisplayBuildingListBlueprint();
	UFUNCTION(BlueprintImplementableEvent)
	void DisplayNormalBlueprint();

	UFUNCTION()
	void MonthlyUpdate();

	virtual void NativeOnInitialized() override;
	virtual void RemoveFromParent() override;

private:
	UFUNCTION()
	void FeatureInfoHovered();
	UFUNCTION()
	void FeatureInfoUnhovered();

	UFUNCTION()
	void CloseWidget();
	UFUNCTION()
	void BuildingSlotClickHandle();

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

	AActor* OwningActor;
	int32 CurrentBuildSlotIndex;
};

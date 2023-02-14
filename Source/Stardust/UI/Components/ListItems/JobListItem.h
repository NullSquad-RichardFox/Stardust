// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Stardust/CoreDataStructs.h"
#include "Blueprint/DragDropOperation.h"
#include "JobListItem.generated.h"


class UImage;
class UTextBlock;

UCLASS()
class STARDUST_API UJobListItem : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void UpdateOccupiedJobs(int32 Change);

protected:
	UFUNCTION(BlueprintCallable)
	void OnItemAddedHandle(UObject* Object);

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	UPROPERTY(EditAnywhere, Category = "Components", meta = (BindWidget))
	UImage* Background;
	UPROPERTY(EditAnywhere, Category = "Components", meta = (BindWidget))
	UImage* JobIcon;
	UPROPERTY(EditAnywhere, Category = "Components", meta = (BindWidget))
	UTextBlock* JobName;
	UPROPERTY(EditAnywhere, Category = "Components", meta = (BindWidget))
	UTextBlock* WorkedJobsText;
	UPROPERTY(EditAnywhere, Category = "Components", meta = (BindWidget))
	UTextBlock* TotalJobsText;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> PopWidgetClass;

public:
	EJobType JobType;
	int32 OccupiedJobs;
	int32 BuildSlotIndex;

private:
	AActor* OwningActor;
};

UCLASS()
class STARDUST_API UListItemDragOperation : public UDragDropOperation
{
	GENERATED_BODY()

public:
	UJobListItem* OriginPtr;
};
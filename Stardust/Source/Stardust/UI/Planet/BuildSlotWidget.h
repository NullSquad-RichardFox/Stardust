// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BuildSlotWidget.generated.h"


class UImage;
class UButton;
class UTextBlock;

UCLASS()
class STARDUST_API UBuildSlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeOnInitialized() override;

	void Preload(AActor* ParentActor, int32 InBuildSlotIndex);

protected:
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

private:
	UFUNCTION()
	void ClickEvent();

protected:
	UPROPERTY(EditAnywhere, Category = "Components", meta = (BindWidget))
	UButton* HitBox;
	UPROPERTY(EditAnywhere, Category = "Components", meta = (BindWidget))
	UImage* BuildSlotImage;
	UPROPERTY(EditAnywhere, Category = "Components", meta = (BindWidget))
	UImage* NotificationIcon;
	UPROPERTY(EditAnywhere, Category = "Components", meta = (BindWidget))
	UTextBlock* NotificationCount;

private:
	AActor* OwningActor;
	int32 BuildSlotIndex;
};

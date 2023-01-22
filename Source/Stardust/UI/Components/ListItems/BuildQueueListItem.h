// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BuildQueueListItem.generated.h"


class UTextBlock;
class UProgressBar;

UCLASS()
class STARDUST_API UBuildQueueListItem : public UUserWidget
{
	GENERATED_BODY()

protected:
	UFUNCTION(BlueprintCallable)
	void OnItemAddedHandle(UObject* Object);

	virtual void NativeOnInitialized() override;

	UPROPERTY(EditAnywhere, Category = "Components", meta = (BindWidget))
	UTextBlock* BuildingNameText;
	UPROPERTY(EditAnywhere, Category = "Components", meta = (BindWidget))
	UTextBlock* QueueIndexText;
	UPROPERTY(EditAnywhere, Category = "Components", meta = (BindWidget))
	UProgressBar* BuildProgress;

private:
	UFUNCTION()
	void DayUpdate();

	FText Name;
	float Total;
	const float* Progress = nullptr;
	int32 QueueIndex;
};

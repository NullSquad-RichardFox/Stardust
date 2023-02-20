// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BuildQueueListItem.generated.h"


class UTextBlock;
class UProgressBar;
class UButton;

UCLASS()
class STARDUST_API UBuildQueueListItem : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetQueueIndex(int32 Index);

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
	UPROPERTY(EditAnywhere, Category = "Components", meta = (BindWidget))
	UButton* CancelButton;

private:
	UFUNCTION()
	void DayUpdate();

	UFUNCTION()
	void CancelBuilding();

	FText Name;
	float Total;
	int32 QueueIndex;
	const float* Progress;
	AActor* OwningActor;
};

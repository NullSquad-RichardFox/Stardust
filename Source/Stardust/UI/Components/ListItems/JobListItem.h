// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "JobListItem.generated.h"


class UImage;
class UTextBlock;

UCLASS()
class STARDUST_API UJobListItem : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UFUNCTION(BlueprintCallable)
	void OnItemAddedHandle(UObject* Object);

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
};

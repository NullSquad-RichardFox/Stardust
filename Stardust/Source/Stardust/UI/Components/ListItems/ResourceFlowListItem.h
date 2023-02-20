// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Stardust/CoreDataStructs.h"
#include "ResourceFlowListItem.generated.h"


class UImage;
class UTextBlock;

UCLASS()
class STARDUST_API UResourceFlowListItem : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetIconToNone();
	void SetText(FText Text);
	void SetResourceType(EResourceType ResourceType);

protected:
	UFUNCTION(BlueprintCallable)
	void OnItemAddedHandle(UObject* Object);

	UPROPERTY(EditAnywhere, Category = "Components", meta = (BindWidget))
	UImage* Background;
	UPROPERTY(EditAnywhere, Category = "Components", meta = (BindWidget))
	UImage* FlowIcon;
	UPROPERTY(EditAnywhere, Category = "Components", meta = (BindWidget))
	UTextBlock* ResourceName;
	UPROPERTY(EditAnywhere, Category = "Components", meta = (BindWidget))
	UTextBlock* ValueText;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RadialProgressBar.generated.h"


class UImage;
class UTextBlock;

UCLASS()
class STARDUST_API URadialProgressBar : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetAlpha(float Alpha);
	void SetCount(int32 Count);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UImage* ProgressBar;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* CountText;
};

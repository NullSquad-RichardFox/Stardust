// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameActor.generated.h"


class AConnection;
class UInputMappingContext;
class UInputAction;

UCLASS(Abstract)
class STARDUST_API AGameActor : public AActor
{
	GENERATED_BODY()
	
public:
	AGameActor();

	bool IsConnectedTo(AGameActor* OtherActor, int32& OutConnectionIndex);
	void SetFindRouteActive(bool Value);

	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<AConnection>> Connections;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnMonthUpdate();
	UFUNCTION()
	virtual void OnDayUpdate();
	UFUNCTION()
	virtual void OnClicked();

private:
	UFUNCTION()
	void OnMouseHovered(AActor* TouchedActor);
	UFUNCTION()
	void OnMouseUnhovered(AActor* TouchedActor);
	UFUNCTION()
	void OnLeftMouseClick();

protected:
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* ActorMeshComponent;

private:
	UPROPERTY()
	UInputMappingContext* GameActorMappingContext;
	UPROPERTY()
	UInputAction* ClickAction;

	bool bMouseOver;

	bool bFindRouteActive;
};

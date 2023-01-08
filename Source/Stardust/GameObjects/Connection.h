// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Connection.generated.h"



class AGameActor;

UCLASS()
class STARDUST_API AConnection : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AConnection();

	void CalculateMeshSize(float Dist);

	AGameActor* GetOtherActor(AGameActor* Actor);

	void SetActors(AGameActor* InStartActor, AGameActor* InEndActor);
	void Select();

public:
	FVector StartPoint, EndPoint;

	float ConnectionValue;
	float DangerLevel;

	float Length;

protected:
	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* Mesh;
	UPROPERTY(EditAnywhere)
	UMaterialInterface* SelectedMaterial;

private:
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<AGameActor> StartActor;
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<AGameActor> EndActor;
};
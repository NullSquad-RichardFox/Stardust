// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "EnhancedInputComponent.h"
#include "SpectatorPlayerPawn.generated.h"


class UInputMappingContext;
class UCameraComponent;
class UFloatingPawnMovement;

UCLASS()
class STARDUST_API ASpectatorPlayerPawn : public APawn
{
	GENERATED_BODY()

public:
	ASpectatorPlayerPawn();

	FORCEINLINE void EnableMovement() { bMovementEnabled = true; }
	FORCEINLINE void DisableMovement() { bMovementEnabled = false; }

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	void HandleMovement(const FInputActionValue& ActionValue);

protected:
	UPROPERTY(EditAnywhere, Category = "Components")
	UCameraComponent* CameraComponent;
	UPROPERTY(EditAnywhere, Category = "Components")
	USceneComponent* SceneComponent;
	UPROPERTY(EditAnywhere, Category = "Components")
	UFloatingPawnMovement* Movement;

private:
	UPROPERTY()
	UInputMappingContext* PlayerMappingContext;
	UPROPERTY()
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float CameraRotation = -50.f;

	UPROPERTY(EditAnywhere, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float MovementSpeed = 1.f;
	UPROPERTY(EditAnywhere, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float ZoomSpeed = 300.f;
	UPROPERTY(EditAnywhere, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	FVector2D ZoomRange = FVector2D(50.f, 5000.f);

	bool bMovementEnabled;
};

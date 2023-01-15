// Fill out your copyright notice in the Description page of Project Settings.


#include "SpectatorPlayerPawn.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Stardust/Libraries/InputFunctionLibrary.h"



ASpectatorPlayerPawn::ASpectatorPlayerPawn()
{
	PrimaryActorTick.bCanEverTick = false;
	bMovementEnabled = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = SceneComponent;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetRelativeRotation(FQuat(FRotator(CameraRotation, 0.f, 0.f)));
	CameraComponent->SetupAttachment(RootComponent);

	Movement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("Movement"));
}


void ASpectatorPlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerMappingContext = NewObject<UInputMappingContext>(this);
	MoveAction = NewObject<UInputAction>(this);

	MoveAction->ValueType = EInputActionValueType::Axis3D;
	
	UInputFunctionLibrary::BindInputAction(this, TEXT("HandleMovement"), ETriggerEvent::Triggered, PlayerMappingContext, MoveAction, EKeys::W,					false,	false,	EInputAxisSwizzle::YXZ);
	UInputFunctionLibrary::BindInputAction(this, TEXT("HandleMovement"), ETriggerEvent::Triggered, PlayerMappingContext, MoveAction, EKeys::S,					true,	false,	EInputAxisSwizzle::YXZ);
	UInputFunctionLibrary::BindInputAction(this, TEXT("HandleMovement"), ETriggerEvent::Triggered, PlayerMappingContext, MoveAction, EKeys::A,					true,	true,	EInputAxisSwizzle::YXZ);
	UInputFunctionLibrary::BindInputAction(this, TEXT("HandleMovement"), ETriggerEvent::Triggered, PlayerMappingContext, MoveAction, EKeys::D,					false,	true,	EInputAxisSwizzle::YXZ);
	UInputFunctionLibrary::BindInputAction(this, TEXT("HandleMovement"), ETriggerEvent::Triggered, PlayerMappingContext, MoveAction, EKeys::MouseScrollUp,		false,	true,	EInputAxisSwizzle::ZYX);
	UInputFunctionLibrary::BindInputAction(this, TEXT("HandleMovement"), ETriggerEvent::Triggered, PlayerMappingContext, MoveAction, EKeys::MouseScrollDown,	true,	true,	EInputAxisSwizzle::ZYX);

	UInputFunctionLibrary::AddInputMapping(PlayerMappingContext, 0);
}

void ASpectatorPlayerPawn::HandleMovement(const FInputActionValue& ActionValue)
{
	if (!bMovementEnabled) return;

	FVector PlanarMovement = FVector(ActionValue.Get<FInputActionValue::Axis3D>().X, ActionValue.Get<FInputActionValue::Axis3D>().Y, 0.f);
	FVector ZoomMovment = FRotator(CameraRotation, 0.f, 0.f).RotateVector(FVector(ActionValue.Get<FInputActionValue::Axis3D>().Z, 0.f, 0.f));

	AddMovementInput(PlanarMovement, MovementSpeed);
	AddActorWorldOffset(ZoomMovment * ZoomSpeed);
}
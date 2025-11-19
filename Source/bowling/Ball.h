// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

// Includes for the user input
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"


#include "Ball.generated.h"

UCLASS()
class BOWLING_API ABall : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABall();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	UPROPERTY(VisibleAnywhere) UCapsuleComponent* CapsuleComp;
	UPROPERTY(VisibleAnywhere) USpringArmComponent* SpringArmComp;
	UPROPERTY(VisibleAnywhere) UCameraComponent* CameraComp;

	UPROPERTY(EditAnywhere, Category = "Input") UInputMappingContext* DefaultMappingContext;
	UPROPERTY(EditAnywhere, Category = "Input") UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, Category = "Input") UInputAction* ThrowAction;


	UPROPERTY(EditAnywhere) float Speed = 50.0f;

	void MoveInput(const FInputActionValue& Value);
	void ThrowInput(const FInputActionValue& Value);
};

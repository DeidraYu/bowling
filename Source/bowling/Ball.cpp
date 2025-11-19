// Fill out your copyright notice in the Description page of Project Settings.

#include "Ball.h"

#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"

// Sets default values
ABall::ABall()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	
	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComp"));
	SetRootComponent(CapsuleComp);

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(CapsuleComp);

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp);
}


// Called when the game starts or when spawned
void ABall::BeginPlay()
{
	Super::BeginPlay();
	
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
			{
				Subsystem->AddMappingContext(DefaultMappingContext, 0);
			}
		}
	}

}

// Called every frame
void ABall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABall::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (UEnhancedInputComponent* EnhancedPlayerInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedPlayerInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABall::MoveInput);
		EnhancedPlayerInputComponent->BindAction(ThrowAction, ETriggerEvent::Triggered, this, &ABall::ThrowInput);

	}

}

void ABall::MoveInput(const FInputActionValue& Value)
{
	float InputValue = Value.Get<float>();

	float DeltaTime = UGameplayStatics::GetWorldDeltaSeconds(GetWorld());

	FVector DeltaLocation = FVector(0.0f, 1.0f, 0.0f);
	DeltaLocation *= Speed * InputValue * DeltaTime;
	AddActorLocalOffset(DeltaLocation, true);
}

void ABall::ThrowInput(const FInputActionValue& Value)
{
}


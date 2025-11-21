// Fill out your copyright notice in the Description page of Project Settings.

#include "Ball.h"

#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"

// Sets default values
ABall::ABall()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SetRootComponent(SphereComp);

	SpringArmAnchor = CreateDefaultSubobject<USceneComponent>(TEXT("SpingArmAnchor"));
	SpringArmAnchor->SetupAttachment(SphereComp);

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(SpringArmAnchor);

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp);


	SpringArmAnchor->PrimaryComponentTick.bCanEverTick = true;
	SpringArmAnchor->PrimaryComponentTick.TickGroup = TG_PostPhysics;
}

void ABall::OnOverlapStopZone(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Controleer op de "STOP" Tag op de overlappende Actor
	if (OtherActor && OtherActor->ActorHasTag(TEXT("STOP")))
	{
		// Gebruik de OverlappedComp om de snelheid op nul te zetten
		if (OverlappedComp->IsSimulatingPhysics())
		{
			// Reset de snelheid van de bal direct
			OverlappedComp->SetPhysicsLinearVelocity(FVector::ZeroVector);
			OverlappedComp->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
		}
	}
}

// Called when the game starts or when spawned
void ABall::BeginPlay()
{
	Super::BeginPlay();
	InitialTransform = GetActorTransform();

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


	if (UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(RootComponent))
	{
		PrimitiveComponent->OnComponentBeginOverlap.AddDynamic(this, &ABall::OnOverlapStopZone);
	}

	StartPosition = GetActorLocation();

}

// Called every frame
void ABall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	// Check if the Anchor component is valid
	if (SpringArmAnchor) // Assuming this is the UPROPERTY reference to your USceneComponent
	{
		FRotator StableRotation;
		if (IsBallRolling)
		{
			StableRotation = FRotator(0.0f, 0.0f, 0.0f);
		}
		else
		{
			// Only use the Yaw angle from the SpringArmAnchor
			FRotator CurrentRotation = SpringArmAnchor->GetComponentRotation();
			StableRotation = FRotator(0.0f, CurrentRotation.Yaw, 0.0f);
		}

		SpringArmAnchor->SetWorldRotation(StableRotation);


		float BallTraveledDistance = FVector::Dist(GetActorLocation(), StartPosition);
		if (!IsCameraFixed && BallTraveledDistance >= CameraMovementStopsAfterDistance)
		{
			IsCameraFixed = true;
			FixedCameraPosition = SpringArmAnchor->GetComponentLocation();
			SpringArmAnchor->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		}

		if (IsCameraFixed)
		{
			SpringArmAnchor->SetWorldLocation(FixedCameraPosition);
		}
	}

	if (!IsBallRolling)
	{
		if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
		{
			FHitResult HitResult;
			if (PlayerController->GetHitResultUnderCursor(ECC_Visibility, false, HitResult))
			{
				DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 5, 12, FColor::Green);
				RotatePlayer(HitResult.ImpactPoint);
			}
		}
	}

}

// Called to bind functionality to input
void ABall::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (UEnhancedInputComponent* EnhancedPlayerInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedPlayerInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABall::MoveInput);
		EnhancedPlayerInputComponent->BindAction(ThrowAction, ETriggerEvent::Started, this, &ABall::ThrowInput);

	}

}

void ABall::MoveInput(const FInputActionValue& Value)
{
	float InputValue = Value.Get<float>();

	float DeltaTime = UGameplayStatics::GetWorldDeltaSeconds(GetWorld());

	FVector DeltaLocation = FVector(0.0f, 1.0f, 0.0f);
	DeltaLocation *= LeftRightSpeed * InputValue * DeltaTime;
	AddActorLocalOffset(DeltaLocation, true);
}

void ABall::ThrowInput(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Display, TEXT("Throw!"));

	if (!IsBallRolling && SphereComp && SphereComp->IsSimulatingPhysics())
	{
		IsBallRolling = true;
		FVector LaunchDirection = GetActorForwardVector();
		FVector Impulse = LaunchDirection * ThrowSpeed * SphereComp->GetMass();
		SphereComp->AddImpulse(Impulse, NAME_None, true);
	}
}

void ABall::RotatePlayer(FVector TargetPosition)
{
	if (!IsBallRolling)
	{
		FVector VectorToTarget = TargetPosition - SphereComp->GetComponentLocation();
		FRotator YawAngle = VectorToTarget.Rotation();
		YawAngle.Pitch = 0;

		FRotator InterpolatedRotation = FMath::RInterpTo(SphereComp->GetComponentRotation(), YawAngle, GetWorld()->GetDeltaSeconds(), 0.5f);

		float MaximalAbsoluteYawAngle = 5.0f;
		if (InterpolatedRotation.Yaw < -MaximalAbsoluteYawAngle)
		{
			InterpolatedRotation.Yaw = -MaximalAbsoluteYawAngle;
		}
		else if (InterpolatedRotation.Yaw > MaximalAbsoluteYawAngle) {
			InterpolatedRotation.Yaw = MaximalAbsoluteYawAngle;
		}

		SphereComp->SetWorldRotation(InterpolatedRotation);
	}
}

void ABall::ResetBallLocation()
{
	SetActorTransform(InitialTransform);
	SpringArmAnchor->AttachToComponent(SphereComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	
	SpringArmAnchor->SetRelativeRotation(FRotator::ZeroRotator);
	SpringArmAnchor->SetRelativeLocation(FVector::ZeroVector);

	SphereComp->SetSimulatePhysics(false);
	IsBallRolling = false;
	IsCameraFixed = false;
	SphereComp->SetPhysicsLinearVelocity(FVector::ZeroVector);
	SphereComp->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
	SphereComp->SetSimulatePhysics(true);

}

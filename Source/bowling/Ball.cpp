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
			// FixedCameraPosition = CameraComp->GetComponentLocation();
			FixedCameraPosition = SpringArmAnchor->GetComponentLocation();
			SpringArmAnchor->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		}

		if (IsCameraFixed)
		{
			// CameraComp->SetWorldLocation(FixedCameraPosition);
			SpringArmAnchor->SetWorldLocation(FixedCameraPosition);
		}


		//if (BallTraveledDistance < CameraMovementStopsAfterDistance)
		//{
		//	// LastSpringArmAnchor = SpringArmAnchor->GetComponentLocation();
		//	LastSpringArmAnchor = CameraComp->GetComponentLocation();

		//}
		//else
		//{
		//	// SpringArmAnchor->SetWorldLocation(LastSpringArmAnchor);
		//	CameraComp->SetWorldLocation(LastSpringArmAnchor);
		//}

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
	// The LaunchSpeed argument will be the value you pass from your controller (e.g., 500.0f).

		// 1. Safety Check: Ensure the component exists and is simulating physics.
	if (!IsBallRolling && SphereComp && SphereComp->IsSimulatingPhysics())
	{
		IsBallRolling = true;
		// 2. Determine the launch direction.
		// GetActorForwardVector() uses the Actor's current Yaw (aiming) direction.
		FVector LaunchDirection = GetActorForwardVector();

		// 3. Calculate the Impulse Vector.
		// Impulse = Mass * Desired Change in Velocity (LaunchSpeed).
		// Multiplying by mass makes the impulse relative to the object's weight.
		FVector Impulse = LaunchDirection * ThrowSpeed * SphereComp->GetMass();

		// 4. Apply the impulse force to the physics body.
		SphereComp->AddImpulse(Impulse, NAME_None, true);

		// Optional: Apply angular impulse for a starting spin (hook/curve)
		// If you want a slight hook, you can add a small torque around the Z (vertical) axis.
		// FVector SpinImpulse = FVector(0.0f, 0.0f, -200.0f); // Adjust Z value for spin amount
		// SphereComp->AddAngularImpulseInDegrees(SpinImpulse, NAME_None, true);
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
		// SetActorRotation(InterpolatedRotation);
	}
}
// Fill out your copyright notice in the Description page of Project Settings.


#include "BowlingGameMode.h"

#include "Kismet/GameplayStatics.h"

ABowlingGameMode::ABowlingGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABowlingGameMode::BeginPlay()
{
	Super::BeginPlay();

    TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APin::StaticClass(), FoundActors);
	PinCount = FoundActors.Num();
	UE_LOG(LogTemp, Display, TEXT("#Pins: %d"), PinCount);

	if (APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0))
	{
		Ball = Cast<ABall>(PlayerPawn);
		if (Ball)
		{
			for (AActor* FoundActor : FoundActors)
			{
                if (APin* Pin = Cast<APin>(FoundActor))
                {
                    Pins.Add(Pin);
                }
			}
		}

	}
}

void ABowlingGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    if (Ball->IsBallRolling)
    {
        if (IsThrowFinished())
        {
            Ball->IsBallRolling = false;

            // Check what pins are fallen or moved:
            int32 RollScore = 0;
            for (APin* Pin : Pins)
            {

                if (!Pin->IsRemoved && Pin->ShouldBeRemoved())
                {
                    RollScore += Pin->ShouldBeRemoved();
                    Pin->ResetPinLocation();
                    Pin->AddActorLocalOffset(FVector(60.0f, 0.0f, 100.0f));
                    Pin->IsRemoved = true;
                }
            }

            UE_LOG(LogTemp, Display, TEXT("RollScore: %d"), RollScore);

        }
    }
    // UE_LOG(LogTemp, Display, TEXT("IsThrowFinished(): %d"), IsThrowFinished());

}

bool ABowlingGameMode::IsThrowFinished()
{
    // The maximum allowed speed before an object is considered "moving."
    const float SleepThreshold2 = 25.0f; // TUNE: e.g., 5.0 cm/s

    // Check the Ball's Velocity (linear and angular)
    if (Ball->SphereComp->GetPhysicsLinearVelocity().SizeSquared() > SleepThreshold2 /* ||
        Ball->SphereComp->GetPhysicsAngularVelocityInDegrees().SizeSquared() > SleepThreshold2 */)
    {
        // The ball is still moving.
        UE_LOG(LogTemp, Display, TEXT("Ball is moving: %f   %f"), Ball->SphereComp->GetPhysicsLinearVelocity().SizeSquared(), Ball->SphereComp->GetPhysicsAngularVelocityInDegrees().SizeSquared());

        return false;
    }

    // Check all Pins' Velocity
    for (APin* Pin : Pins)
    {
        // Check if the Pin is simulating physics (i.e., not destroyed)
        if (Pin->BaseMesh->IsSimulatingPhysics())
        {
            // Use GetPhysicsLinearVelocity().SizeSquared() for faster comparison
            if (Pin->BaseMesh->GetPhysicsLinearVelocity().SizeSquared() > SleepThreshold2 /* ||
                Pin->BaseMesh->GetPhysicsAngularVelocityInDegrees().SizeSquared() > SleepThreshold2 */)
            {
                // At least one pin is still moving.
                UE_LOG(LogTemp, Display, TEXT("A Pin is moving"));

                return false;
            }
        }
    }

    // If we passed all checks, nothing is moving.
    return true;
}
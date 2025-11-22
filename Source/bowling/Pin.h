// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Pin.generated.h"

UCLASS()
class BOWLING_API APin : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APin();

	UPROPERTY(VisibleAnywhere) UStaticMeshComponent* BaseMesh;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	FTransform InitialTransform;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	bool IsRemoved = false;

	// Check if the pin is fallen or moved to remove it from the frame.
	bool ShouldBeRemoved() const;

	// Store and hide the pin until the frame is over.
	void Store();

	// Put the pin back at is starting position (undo the effect of the Store() call)
	void Initialize();
};

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

	bool ShouldBeRemoved() const;
	void ResetPinLocation();


};

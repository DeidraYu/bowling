// Fill out your copyright notice in the Description page of Project Settings.


#include "Pin.h"

// Sets default values
APin::APin()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	SetRootComponent(BaseMesh);
}

// Called when the game starts or when spawned
void APin::BeginPlay()
{
	Super::BeginPlay();
	InitialTransform = GetActorTransform();

}

// Called every frame
void APin::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool APin::ShouldBeRemoved() const
{
	bool IsFallen = FVector::DotProduct(GetActorUpVector(), FVector::UpVector) < FMath::Cos(FMath::DegreesToRadians(5.0f));
	float Distance2D = FVector::Dist2D(GetActorLocation(), InitialTransform.GetLocation());
	bool IsMoved = Distance2D > 2.0f;

	return IsFallen || IsMoved;
}

void APin::Store()
{
	BaseMesh->SetSimulatePhysics(false);
	BaseMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FVector StorageLocation(0.0f, 0.0f, -10000.0f);
	SetActorLocation(StorageLocation, false, nullptr, ETeleportType::TeleportPhysics);
	SetActorTickEnabled(false);
}

void APin::Initialize()
{
	SetActorTransform(InitialTransform, false, nullptr, ETeleportType::TeleportPhysics);
	BaseMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BaseMesh->SetSimulatePhysics(true);
	SetActorHiddenInGame(false);
	SetActorTickEnabled(true);
	IsRemoved = false;
}
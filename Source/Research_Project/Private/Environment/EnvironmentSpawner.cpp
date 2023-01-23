// Fill out your copyright notice in the Description page of Project Settings.


#include "Environment/EnvironmentSpawner.h"
#include "Environment/EnvironmentActor.h"
#include "Components/BoxComponent.h"

AEnvironmentSpawner::AEnvironmentSpawner()
{
	PrimaryActorTick.bCanEverTick = true;
	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Spawn Box"));
	SetRootComponent(Box);
}

void AEnvironmentSpawner::BeginPlay()
{
	Super::BeginPlay();
	World = GetWorld();
	SpawnActors();
	
}

void AEnvironmentSpawner::SpawnActors()
{
	if (TotalToSpawn > 0) {
		for (int32 i = 0; i < TotalToSpawn; i++) {
			FVector Location = FVector(CalculatePerlinX(), CalculatePerlinY(), 200.f);
			SpawnActorAtLocation(Location);
		}
	}
}

void AEnvironmentSpawner::SpawnActorAtLocation(FVector Location)
{
	if (World && ActorsToSpawn.Num() > 0) {
		const uint8 Selection = FMath::RandRange(0, ActorsToSpawn.Num() - 1);
		World->SpawnActor<AEnvironmentActor>(ActorsToSpawn[Selection], Location, FRotator(0.f, 0.f, 0.f));
	}
}

float AEnvironmentSpawner::CalculatePerlinX()
{
	float RandomDisplacementX = FMath::RandRange((PerlinSeed * MinPerlin) - MaxSpawnRange, (PerlinSeed * MaxPerlin) + MaxSpawnRange);
	float DisplacementX = FMath::Clamp(RandomDisplacementX, -MaxSpawnRange, MaxSpawnRange);
	if (FMath::Abs(DisplacementX) == MaxSpawnRange) {
		DisplacementX = FMath::RandRange(- MaxSpawnRange, MaxSpawnRange);
	}
	return DisplacementX;
}

float AEnvironmentSpawner::CalculatePerlinY()
{
	float RandomDisplacementY = FMath::RandRange((PerlinSeed * MinPerlin) - MaxSpawnRange, (PerlinSeed * MaxPerlin) + MaxSpawnRange);
	float DisplacementY = FMath::Clamp(RandomDisplacementY, -MaxSpawnRange, MaxSpawnRange);
	if (FMath::Abs(DisplacementY) == MaxSpawnRange) {
		DisplacementY = FMath::RandRange(-MaxSpawnRange, MaxSpawnRange);
	}
	return DisplacementY;
}

void AEnvironmentSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


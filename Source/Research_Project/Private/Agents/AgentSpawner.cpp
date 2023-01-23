// Fill out your copyright notice in the Description page of Project Settings.


#include "Agents/AgentSpawner.h"
#include "Agents/Agent.h"
#include "Components/BoxComponent.h"
#include "Research_Project/DebugMacros.h"

AAgentSpawner::AAgentSpawner()
{
	PrimaryActorTick.bCanEverTick = true;
	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Agent Spawn Box"));
	SetRootComponent(Box);
}

void AAgentSpawner::BeginPlay()
{
	Super::BeginPlay();
	World = GetWorld();
	SpawnActors();
	
}

void AAgentSpawner::SpawnActors()
{
	if (TotalToSpawn > 0) {
		for (uint8 i = 0; i < TotalToSpawn; i++) {
			FVector Location = FVector(CalculatePerlinX(), CalculatePerlinY(), 50.f);
			SpawnActorAtLocation(Location);
		}
	}
}

void AAgentSpawner::SpawnActorAtLocation(FVector Location)
{
	if (World && AgentsToSpawn.Num() > 0) {
		const uint8 selection = FMath::RandRange(0, AgentsToSpawn.Num() - 1);
		World->SpawnActor<AAgent>(AgentsToSpawn[selection], Location, FRotator(0.f, 0.f, 0.f));
	}
}

float AAgentSpawner::CalculatePerlinX()
{
	float RandomDisplacementX = FMath::RandRange((PerlinSeed * MinPerlin) - MaxSpawnRange, (PerlinSeed * MaxPerlin) + MaxSpawnRange);
	const float DisplacementX = FMath::Clamp(RandomDisplacementX, -MaxSpawnRange, MaxSpawnRange);
	return DisplacementX;
}

float AAgentSpawner::CalculatePerlinY()
{
	float RandomDisplacementY = FMath::RandRange((PerlinSeed * MinPerlin) - MaxSpawnRange, (PerlinSeed * MaxPerlin) + MaxSpawnRange);
	const float DisplacementY = FMath::Clamp(RandomDisplacementY, -MaxSpawnRange, MaxSpawnRange);
	return DisplacementY;
}

void AAgentSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAgentSpawner::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


// Fill out your copyright notice in the Description page of Project Settings.


#include "Environment/EnvironmentSpawner.h"
#include "Environment/EnvironmentActor.h"
#include "Research_Project/DebugMacros.h"
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
	StartRenewTimer();
}

void AEnvironmentSpawner::SpawnActors()
{
	if (TotalToSpawn > 0) {
		PRINT_STRING(TEXT("Spawning environment actors"))
		for (int32 i = 0; i < TotalToSpawn; i++) {
			SpawnActorAtLocation();
		}
	}
}

void AEnvironmentSpawner::SpawnActorAtLocation()
{
	if (World && ActorsToSpawn.Num() > 0) {
		const uint8 Selection = FMath::RandRange(0, ActorsToSpawn.Num() - 1);
		AEnvironmentActor* NewActor;
		do {
			FVector NewLocation = FVector(CalculatePerlinX(), CalculatePerlinY(), 200.f);
			NewActor = World->SpawnActor<AEnvironmentActor>(ActorsToSpawn[Selection], NewLocation, FRotator(0.f, 0.f, 0.f));
		} while (NewActor == nullptr);
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

void AEnvironmentSpawner::RenewEnvironment()
{
	if (World == nullptr) return;

	ClearRenewTimer();

	TArray<TEnumAsByte<EObjectTypeQuery>> OverlapObjectTypes;
	OverlapObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_GameTraceChannel2));

	TArray<AActor*>ActorsToIgnore;
	TArray<AActor*>OutActors;

	PRINT_STRING_BLUE("OutActors renew environment")
	PRINT_INT(OutActors.Num())
	UKismetSystemLibrary::BoxOverlapActors(World, GetActorLocation(), Box->GetScaledBoxExtent(), OverlapObjectTypes, AEnvironmentActor::StaticClass(), ActorsToIgnore, OutActors);
	for (int32 NumOfActors = 0; NumOfActors < OutActors.Num(); NumOfActors++) {
		AEnvironmentActor* FoundActor = Cast<AEnvironmentActor>(OutActors[NumOfActors]);
		if (FoundActor)
		{
			TSubclassOf<AEnvironmentActor> FoundActorClass = FoundActor->GetClass();
			if (ActorsToSpawn.Contains(FoundActorClass)) {
				FoundActor->Destroy();

			}
		}
	}

	SpawnActors();
	StartRenewTimer();
}

void AEnvironmentSpawner::StartRenewTimer()
{
	GetWorldTimerManager().SetTimer(RenewHandle, this, &AEnvironmentSpawner::RenewEnvironment, RenewTime);
}

void AEnvironmentSpawner::ClearRenewTimer()
{
	GetWorldTimerManager().ClearTimer(RenewHandle);
}

void AEnvironmentSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
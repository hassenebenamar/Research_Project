// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnvironmentSpawner.generated.h"

class AEnvironmentActor;
class UBoxComponent;

UCLASS()
class RESEARCH_PROJECT_API AEnvironmentSpawner : public AActor
{
	GENERATED_BODY()
	
public:	

	AEnvironmentSpawner();
	virtual void Tick(float DeltaTime) override;

	UWorld* World;

protected:
	virtual void BeginPlay() override;

	//research functions
	void SpawnActors();

private:
	//actor to spawn

	UPROPERTY(EditAnywhere)
		TArray<TSubclassOf<AEnvironmentActor>> ActorsToSpawn;

	UBoxComponent* Box;

	// spawning mathematical paramaters

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	int32 TotalToSpawn;

	UPROPERTY(EditAnywhere)
		float MaxSpawnRange;

	UPROPERTY(EditAnywhere)
		float PerlinSeed;

	UPROPERTY(EditAnywhere)
		float MinPerlin;

	UPROPERTY(EditAnywhere)
		float MaxPerlin;

	//spawn functions

	void SpawnActorAtLocation(FVector Location);
	float CalculatePerlinX();
	float CalculatePerlinY();

};

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

	void SpawnDestroyable();

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

	UPROPERTY(EditAnywhere, Category = "Environment Spawning")
		float MaxSpawnRange;

	UPROPERTY(EditAnywhere, Category = "Environment Spawning")
		float PerlinSeed;

	UPROPERTY(EditAnywhere, Category = "Environment Spawning")
		float MinPerlin;

	UPROPERTY(EditAnywhere, Category = "Environment Spawning")
		float MaxPerlin;

	//spawn functions

	void SpawnActorAtLocation();
	float CalculatePerlinX();
	float CalculatePerlinY();

	//timers
	void RenewEnvironment();
	
	FTimerHandle RenewHandle;
	UPROPERTY(EditAnywhere, Category = "Environment Spawning")
	float RenewTime;
	void StartRenewTimer();
	void ClearRenewTimer();
};

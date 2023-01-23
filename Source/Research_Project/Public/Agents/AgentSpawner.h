// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AgentSpawner.generated.h"

class AAgent;
class UBoxComponent;

UCLASS()
class RESEARCH_PROJECT_API AAgentSpawner : public ACharacter
{
	GENERATED_BODY()

public:

	AAgentSpawner();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UWorld* World;
protected:

	virtual void BeginPlay() override;

	//research functions
	void SpawnActors();
	void SpawnActorAtLocation(FVector Location);

private:

	//actors to spawns
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<AAgent>> AgentsToSpawn;

	UBoxComponent* Box;
	// spawning mathematical paramaters

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	uint8 TotalToSpawn;

	UPROPERTY(EditAnywhere)
		float MaxSpawnRange;

	UPROPERTY(EditAnywhere)
		float PerlinSeed;

	UPROPERTY(EditAnywhere)
		float MinPerlin;

	UPROPERTY(EditAnywhere)
		float MaxPerlin;

	//spawn functions

	float CalculatePerlinX();
	float CalculatePerlinY();

};

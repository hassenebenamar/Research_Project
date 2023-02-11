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
	void SpawnActorsStart();
	void SpawnActorGenerated(AAgent* ActorToSpawn, FVector Location);

private:

	//actors to spawns
	UPROPERTY(EditAnywhere)
	TSubclassOf<AAgent> ClassToSpawn;
	UPROPERTY(EditAnywhere)
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

	//genetic algorithm

	UPROPERTY(EditAnywhere, Category = "Genetic Algorithm")
	float CrossoverChance;
	UPROPERTY(EditAnywhere, Category = "Genetic Algorithm")
	float MutationChance;

	UPROPERTY(EditAnywhere, Category = "Genetic Algorithm")
	AAgent* FirstCandidate;
	UPROPERTY(EditAnywhere, Category = "Genetic Algorithm")
	AAgent* SecondCandidate;
	UPROPERTY(EditAnywhere, Category = "Genetic Algorithm")
	AAgent* ChildOne;
	UPROPERTY(EditAnywhere, Category = "Genetic Algorithm")
	AAgent* ChildTwo;
	UPROPERTY(EditInstanceOnly, Category = "Genetic Algorithm")
	TArray<AAgent*> Population;
	UPROPERTY(EditInstanceOnly, Category = "Genetic Algorithm")
	TArray<AAgent*> NewPopulation;

	TArray<AActor*> ActorsToIgnore;
	TArray<AActor*> OutActors;

	void Selection();
	void Crossover();
	bool CheckPopulation();
	bool CheckNewPopulation();
	void MutateChildren();
	void DestroyParents(AAgent* FirstParent, AAgent* SecondParent);
	void RemoveParentsFromPopulation(AAgent* FirstParent, AAgent* SecondParent);
	void SpawnChildren();
	void Mutation(AAgent* AgentToMutate);
	TArray<AAgent*> GetAllAgents();

	//timer related functions

	FTimerHandle GenerationTimer;
	UPROPERTY(EditAnywhere, Category = "Genetic Algorithm")
	float GenerationTime;

	void StartGenerationTimer();
	void ClearGenerationTimer();
};

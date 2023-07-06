// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SpecieStatus.h"
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AAgent> ClassToSpawn;
protected:

	virtual void BeginPlay() override;

	//research functions
	void SpawnActorsStart();
	void SpawnOffspring(TArray<AAgent*> ActorsToSpawn);

private:

	//actors to spawns
	
	UPROPERTY(EditAnywhere)
	UBoxComponent* Box;
	// spawning mathematical paramaters

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	uint8 TotalToSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	uint8 TerminationCriterion; //should be set to 50 generations

	UPROPERTY(EditAnywhere)
	float MaxSpawnRange;

	UPROPERTY(EditAnywhere)
	float PerlinSeed;

	UPROPERTY(EditAnywhere)
	float MinPerlin;

	UPROPERTY(EditAnywhere)
	float MaxPerlin;

	//spawn functions
	UFUNCTION(BlueprintCallable)
	float CalculatePerlinX();
	UFUNCTION(BlueprintCallable)
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Genetic Algorithm", meta = (AllowPrivateAccess))
	TArray<AAgent*> Population;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Genetic Algorithm", meta = (AllowPrivateAccess))
	TArray<AAgent*> NewPopulation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Genetic Algorithm", meta = (AllowPrivateAccess))
	TArray<AAgent*> OffspringPopulation;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Genetic Algorithm", meta = (AllowPrivateAccess))
	TArray<AAgent*>ActorsSpawned;

	TArray<AActor*> ActorsToIgnore;
	TArray<AActor*> OutActors;

	void Selection();
	void RemoveLastParentIfNeeded();
	void Crossover();
	bool CheckNewPopulation();
	void MutateChildren();
	void Mutation(AAgent* AgentToMutate);
	TArray<AAgent*> GetAllAgents();

	//timer related functions

	FTimerHandle GenerationTimer;
	UPROPERTY(EditAnywhere, Category = "Genetic Algorithm")
	float GenerationTime;

	void StartGenerationTimer();
	void ClearGenerationTimer();

	//save files functions

	void SaveAgentsAttributes();
};

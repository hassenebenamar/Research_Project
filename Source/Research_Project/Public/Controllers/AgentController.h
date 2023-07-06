// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AgentController.generated.h"

class AAgent;
class ACombatSystem;
class AFoodSearchSystem;
class AWanderingSystem;
class AEnvironmentActor;
class UBehaviorTreeComponent;
/**
 * 
 */
UCLASS()
class RESEARCH_PROJECT_API AAgentController : public AAIController
{
	GENERATED_BODY()
	
public:
	//default constructor
	AAgentController();

	void AgentBehaviorChoosing(AAgent* CallerAgent);
	void PossessAndGetUp(AAgent* CallerAgent);
	void PossessAndSearchFood(AAgent* CallerAgent, AEnvironmentActor* Food);
	void PossessAndMoveInTerritory(AAgent* CallerAgent);
	void GetUpOrSleep(AAgent* CallerAgent);
	void RegenEnergyAndHealth(float& Health, float MaxHealth, float& Energy);
	bool bHealthEnergyCheck(AAgent* CallerAgent);
	double GetDistance(AAgent* CallerAgent, AAgent* Enemy);
	
private:

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	AWanderingSystem* WanderingSystem;
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	AFoodSearchSystem* FoodSearchSystem;
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	ACombatSystem* CombatSystem;

	void ReactToEnemy(AAgent* CallerAgent, AAgent* Enemy);
	void PossessAgentAndEnemy(AAgent* CallerAgent, AAgent* Enemy);
};

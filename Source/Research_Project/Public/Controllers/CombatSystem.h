// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "CombatSystem.generated.h"

class AAgent;
class UAgentAttribute;
/**
 * 
 */
UCLASS()
class RESEARCH_PROJECT_API ACombatSystem : public AAIController
{
	GENERATED_BODY()

public:

	//default contructor
	ACombatSystem();

	AAgent* DetectEnemyInRange(AAgent* CallerAgent);
	void Chase(AAgent* CallerAgent, AAgent* Enemy);
	void LoseEnergy(AAgent* CallerAgent);
	void RunAway(AAgent* CallerAgent, AAgent* Enemy);
	void Attack(AAgent* CallerAgent, AAgent* Enemy);

	bool HasEnemy(AAgent* Enemy);
	bool IsPrey(AAgent* CallerAgent, AAgent* Enemy);
	bool IsPredator(AAgent* CallerAgent, AAgent* Enemy);

public:
	//movement 
	template<typename T>
	inline void MoveToTarget(T* Target)
	{
		if (this == nullptr || Target == nullptr) return;
		FAIMoveRequest MoveRequest;
		MoveRequest.SetGoalActor(Target);
		MoveRequest.SetAcceptanceRadius(30.f);
		MoveTo(MoveRequest);
	}

private:

	void SelectRunAwayLocation(AAgent* CallerAgent, AAgent* Enemy);
	FVector CalculateRunAwayLocation(AAgent* CallerAgent, AAgent* Enemy);
	void HandleDamage(AAgent* Agent, AAgent* Enemy);
};

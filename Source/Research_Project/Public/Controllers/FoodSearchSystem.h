// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "FoodSearchSystem.generated.h"

class AEnvironmentActor;
class AAgent;
/**
 * 
 */
UCLASS()
class RESEARCH_PROJECT_API AFoodSearchSystem : public AAIController
{
	GENERATED_BODY()

public:

	//default constructor
	AFoodSearchSystem();

	AEnvironmentActor* DetectFoodInRange(AAgent* CallerAgent);
	void GoToFood(AAgent* CallerAgent, AEnvironmentActor* Food);
	void EatFood(AAgent* CallerAgent, AEnvironmentActor* Food);
	bool HasFood(AEnvironmentActor* Food);

	//movement 
	void MoveToTarget(AAgent* CallerAgent, AEnvironmentActor* Food);
	void LoseEnergy(AAgent* CallerAgent);

};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Research_Project/DebugMacros.h"
#include "WanderingSystem.generated.h"

class AAgent;
/**
 * 
 */
UCLASS()
class RESEARCH_PROJECT_API AWanderingSystem : public AAIController
{
	GENERATED_BODY()

public:

	//default constructor
	AWanderingSystem();

	void Sleep(AAgent* CallerAgent);
	void GetUp(AAgent* CallerAgent);
	
	//movement 

	FVector CalculateNavigableLocationInRadius(AAgent* CallerAgent);
	void MoveInTerritory(AAgent* CallerAgent);
	void LoseEnergy(AAgent* CallerAgent);
	void MoveToCertainLocation(FVector Location);

};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SimulationMode.generated.h"

class AAgent;
class AAgentSpawner;
/**
 * 
 */
UCLASS()
class RESEARCH_PROJECT_API ASimulationMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	//need to be extended for more freedom
	ASimulationMode();
	
};

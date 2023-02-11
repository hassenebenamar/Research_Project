// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Agents/Agent.h"
#include "Deer.generated.h"

/**
 * 
 */
UCLASS()
class RESEARCH_PROJECT_API ADeer : public AAgent
{
	GENERATED_BODY()

public:
	ADeer();

private:
	UCapsuleComponent* Capsule = ACharacter::GetCapsuleComponent();
};

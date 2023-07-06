// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Agents/Agent.h"
#include "Fox.generated.h"

/**
 * 
 */
UCLASS()
class RESEARCH_PROJECT_API AFox : public AAgent
{
	GENERATED_BODY()
	
public:
	AFox();

	UCapsuleComponent* Capsule = ACharacter::GetCapsuleComponent();

};

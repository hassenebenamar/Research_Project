// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DefaultPawn.h"
#include "Admin.generated.h"

/**
 * 
 */
UCLASS()
class RESEARCH_PROJECT_API AAdmin : public ADefaultPawn
{
	GENERATED_BODY()

public:

	AAdmin();
	void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
};

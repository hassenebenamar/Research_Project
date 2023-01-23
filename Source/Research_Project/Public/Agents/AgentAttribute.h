// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Environment/EnvStatus.h"
#include "Agents/SpecieStatus.h"
#include "AgentAttribute.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RESEARCH_PROJECT_API UAgentAttribute : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAgentAttribute();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<uint8> DNA;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EAgentSpecie AgentSpecie;
protected:
	virtual void BeginPlay() override;

private:
	//stats
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess), category = "Agent Stats")
		float AgentSpeed;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess), category = "Agent Stats")
		float AgentDamage;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess), category = "Agent Stats")
		float AgentHealth;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess), category = "Agent Stats")
		float AgentWidth;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess), category = "Agent Stats")
		float AgentHeight;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess), category = "Agent Stats")
		uint8 AgentSize;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess), category = "Agent Stats")
		uint8 Energy;
	//environment variables
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	EEnvType IdealEnvironment;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	EEnvType EnvironmentType;

	//research variables

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess))
		float SurvivabilityScore;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess))
		bool bWasChosen;


public:

	/*Species status check*/
	bool bIsAlive();
	bool bIsDead();
	bool bIsHunting();
	bool bIsAttacking();
	bool bIsActive();
	bool bIsSleeping();

	EAnimalState AnimalState;

	/*Environment check*/
	bool bIsEnvIdeal();

	// stats functions

	float CalculateDamage(float Size);
	float CalculateHealth(float Size, float Damage);
	float CalculateSize(float CapsuleHalfRadius);

	float CalculateSurvivability(float Speed, float Damage, float Size, float Health);

	UFUNCTION(BlueprintCallable)
	FORCEINLINE EAnimalState GetAgentState() { return AnimalState; }
};

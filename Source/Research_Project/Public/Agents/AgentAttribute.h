// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Environment/EnvStatus.h"
#include "Agents/SpecieStatus.h"
#include "Agents/AttributeProperties.h"
#include "Containers/Map.h"
#include "AgentAttribute.generated.h"

class AAgent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RESEARCH_PROJECT_API UAgentAttribute : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAgentAttribute();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, category = "Agent Stats")
	int64 AgentID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, category = "Agent Stats")
	TArray<uint8> DNA;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EAgentSpecie AgentSpecie;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess), category = "Agent Stats")
	float SightRange;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess), category = "Agent Stats")
	float SightOffset;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess), category = "Agent Stats")
	float SightRadius;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess), category = "Agent Stats")
	float HearingThreshold;
	
	//stats
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess), category = "Agent Stats")
		float AgentHealth;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess), category = "Agent Stats")
		float MaxAgentHealth;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess), category = "Agent Stats")
		float AgentSpeed;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess), category = "Agent Stats")
		float AgentDamage;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess), category = "Agent Stats")
		uint8 AgentSize;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess), category = "Agent Stats")
		uint8 Energy;

	//research variables

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess), category = "Agent Stats")
	float SurvivabilityScore;
	//init stats

	void InitializeNewStats(AAgent* AgentToInit);
	void InitializeBaseStats(AAgent* AgentToInit);
	void InitializeInt64Stats(AAgent* Agent);
	
protected:
	virtual void BeginPlay() override;

	void CalculateBaseStats(AAgent* Agent);
	void CalculateStatPerAS(FString& ClassName, AAgent* Agent, EFloatPropertyName PropertyEnum);
	TArray<float> CalculateSenseBonus(AAgent* Agent, FString& ClassName);
	void InitializeFloatStats(AAgent* Agent);
	void InitializeDNA(AAgent* AgentToInit);

private:
	
	//environment variables
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess), category = "Agent Stats")
	EEnvType IdealEnvironment;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess), category = "Agent Stats")
	EEnvType EnvironmentType;

	TArray<FProperty*> Properties;
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

	float CalculateSize(AAgent* AgentToInit);
	float CalculateDamage(float Size);
	float CalculateHealth(float Size, float Damage);
	float CalculateSpeed(float Size);

	float CalculateSurvivability(float Speed, float Damage, float Size, float Health);

	UFUNCTION(BlueprintCallable)
	FORCEINLINE EAnimalState GetAgentState() { return AnimalState; }
	FORCEINLINE void SetAgentState(EAnimalState State) { AnimalState = State; }
};

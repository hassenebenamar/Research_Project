// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Research_Project/DebugMacros.h"
#include "SpecieStatus.h"
#include "Environment/EnvStatus.h"
#include "Agent.generated.h"

class UAgentAttribute;
class AEnvironmentActor;
class AAgentBehaviorTree;

UCLASS()
class RESEARCH_PROJECT_API AAgent : public ACharacter
{
	GENERATED_BODY()

public:

	//default constructor
	AAgent();

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UWorld* World;
	//atributes
	UPROPERTY(VisibleAnywhere)
	UAgentAttribute* Attributes;

	void CopyObject(const AAgent& ObjectToCopy);
	UPROPERTY(EditAnywhere)
	float CombatRadius;
	
	//traces
	TArray<AEnvironmentActor*> GetPossibleFood();
	TArray<AAgent*> GetPossibleEnemies();

	/*Species status check*/
	bool IsAlive();
	bool IsDead();
	bool IsHunting();
	bool IsAttacking();
	bool IsActive();
	bool IsSleeping();

	//getters and setters
	UFUNCTION(BlueprintCallable)
	FORCEINLINE TArray <EAgentSpecie > GetPredators() { return Predators; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE TArray <EAgentSpecie > GetPrey() { return Preys; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE TArray <EEnvActorType> GetFood() { return GreenFood; }

	void Die();
	bool CheckOverlap(AActor* OverlapActor);

protected:
	virtual void BeginPlay() override;

	//play animations (usable by children)

	UPROPERTY(VisibleAnywhere, Category = "Agent Animations")
	TArray<class UAnimSequence*> AnimArray;
	UPROPERTY(EditAnywhere, Category = "Agent Animations")
	class UAnimMontage* AttackMontage;
	UPROPERTY(EditAnywhere, Category = "Agent Animations")
	UAnimMontage* DeathMontage;

	void PlaySleepAnim();
	void PlayGetUpAnim();
	void PlayRunAwayAnim();
	void PlayHuntingAnim();
	void PlayEatingAnim();

	//red targets
	UPROPERTY(EditAnywhere, category = "Red Targets")
	TArray<EAgentSpecie> Predators;
	UPROPERTY(EditAnywhere, category = "Red Targets")
	TArray<EAgentSpecie> Preys;

private:
	//AI Behavior
	UPROPERTY(EditAnywhere, category = "Red Targets")
	TArray<AAgent*> Targets;
	//green targets
	UPROPERTY(EditAnywhere, category = "Green Targets")
	TArray<EEnvActorType> GreenFood;
	UPROPERTY(EditAnywhere, category = "Green Targets")
	TArray<AEnvironmentActor*> GreenTargets;

	UPROPERTY(EditAnywhere, category = "Behavior")
	FTimerHandle BehaviorTimer;
	UFUNCTION(BlueprintCallable)
	void CallBehaviorTree();
	UFUNCTION(BlueprintCallable)
	void StartBehaviorTimer();
	UFUNCTION(BlueprintCallable)
	void ClearBehaviorTimer();
};


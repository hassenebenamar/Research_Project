// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SpecieStatus.h"
#include "Perception/AIPerceptionTypes.h"
#include "Environment/EnvStatus.h"
#include "Agent.generated.h"

class UAgentAttribute;
class AEnvironmentActor;
class UAIPerceptionComponent;
class UAISenseConfig_Sight;
class UAISenseConfig_Hearing;
class UPawnSensingComponent;
class UAISenseConfig_Sight;
class AAIController;
struct FAIStimulus;

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

	//Senses config
	UPROPERTY(EditAnywhere, Category = "AI Senses")
		UAISenseConfig_Sight* SightConfig;
	UPROPERTY(EditAnywhere, Category = "AI Senses")
		UAISenseConfig_Hearing* HearingConfig;
	UPROPERTY(EditAnywhere, Category = "AI Senses")
		UAIPerceptionComponent* PerceptionComponent;

protected:
	virtual void BeginPlay() override;

	/*Species status check*/
	bool IsAlive();
	bool IsDead();
	bool IsHunting();
	bool IsAttacking();
	bool IsActive();
	bool IsSleeping();

	UPROPERTY(EditAnywhere)
	float CombatRadius;
	//functions 

	// Red targets functions (callable by children classes)

	float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	bool IsInCombatRange(AAgent* Target);

	UFUNCTION(BlueprintCallable)
	void HostileAgentDetected(AActor* DetectedAgent, FAIStimulus Stimulus);
	void ReactToEnemy();

	//Green targets functions (usable by children)

	bool SphereTrace();
	void ReactToGreenTarget();

	// Moving function (can be called no matter the typename as long as it's an actor)

	template<typename T>
	void MoveToTarget(T* SeenTarget);

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
	void PlayAttackMontage();
	void PlayDeathMontage();


private:
	//AI Behavior

	UPROPERTY(EditAnywhere)
	AAIController* AgentController;

	//red targets
	UPROPERTY(EditAnywhere, category = "Red Targets")
	TArray<EAgentSpecie> Predators;
	UPROPERTY(EditAnywhere, category = "Red Targets")
	TArray<EAgentSpecie> Preys;
	UPROPERTY(VisibleAnywhere, category = "Red Targets")
	AAgent* Enemy;

	//green targets
	UPROPERTY(EditAnywhere, category = "Green Targets")
	TArray<EEnvActorType> GreenFood;
	TArray<AEnvironmentActor*> GreenTargets;

	void GetRunAwayLocation();

	void Sleep();
	void GetUp();
	void Hunt();
	void Attack();
	void RunAway();
	void Die();

	//timers

	UPROPERTY(EditAnywhere)
	float AttackTime;

	FTimerHandle AttackTimer;
	
	void StartAttackTimer();
	void ClearAttackTimer();

	FTimerHandle GreenTargetTimerHandler;
	void StartGreenTargetTimer();
	void ClearGreenTargetTimer();

	// interaction with environment

	UPROPERTY(VisibleAnywhere, category = "Interaction with Environment")
	AEnvironmentActor* OverlappedEnvActor;

	UFUNCTION(BlueprintCallable)
	void OnActorOverlap(AActor* OverlappedActor, AActor* OtherActor);

};

template<typename T>
inline void AAgent::MoveToTarget(T* Target)
{
	if (AgentController == nullptr || Target == nullptr) return;
	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalActor(Target);
	MoveRequest.SetAcceptanceRadius(30.f);
	AgentController->MoveTo(MoveRequest);
}

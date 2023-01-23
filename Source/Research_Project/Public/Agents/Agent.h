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
class UPawnSensingComponent;
class UAISenseConfig_Sight;
class AAIController;
struct FAIStimulus;

UCLASS()
class RESEARCH_PROJECT_API AAgent : public ACharacter
{
	GENERATED_BODY()

public:
	AAgent();

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UWorld* World;

protected:
	virtual void BeginPlay() override;

	/*Species status check*/
	bool IsAlive();
	bool IsDead();
	bool IsHunting();
	bool IsAttacking();
	bool IsActive();
	bool IsSleeping();

	//atributes
	UPROPERTY(VisibleAnywhere)
	UAgentAttribute* Attributes;

	UPROPERTY(EditAnywhere)
	float CombatRadius;
	//functions 
	float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	bool IsInCombatRange(AAgent* Target);
	UFUNCTION(BlueprintCallable)
	void ActorSeen(AActor* SeenActor, FAIStimulus Stimulus);

	void ReactToEnemy();

	template<typename T>
	void MoveToTarget(T* SeenTarget);

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

	// interaction with environment

	UPROPERTY(VisibleAnywhere, category = "Interaction with Environment")
	AEnvironmentActor* OverlappedEnvActor;

	UFUNCTION(BlueprintCallable)
	void OnActorOverlap(AActor* OverlappedActor, AActor* OtherActor);

	//play animations

	UPROPERTY(EditAnywhere, Category = "Agent Animations")
	class UAnimSequence* SleepAnim;
	UPROPERTY(EditAnywhere, Category = "Agent Animations")
	UAnimSequence* GetUpAnim;
	UPROPERTY(EditAnywhere, Category = "Agent Animations")
	UAnimSequence* RunAwayAnim;
	UPROPERTY(EditAnywhere, Category = "Agent Animations")
	UAnimSequence* HuntingAnim;
	UPROPERTY(EditAnywhere, Category = "Agent Animations")
	UAnimSequence* EatingAnim;
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

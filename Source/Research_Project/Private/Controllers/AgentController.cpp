// Fill out your copyright notice in the Description page of Project Settings.


#include "Controllers/AgentController.h"
#include "Controllers/Controllers.h"
#include "Agents/AgentsHeader.h"
#include "Environment/EnvironmentActor.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

AAgentController::AAgentController()
{
	WanderingSystem = CreateDefaultSubobject<AWanderingSystem>(TEXT("Wandering System"));
	FoodSearchSystem = CreateDefaultSubobject<AFoodSearchSystem>(TEXT("Food Search System"));
	CombatSystem = CreateDefaultSubobject<ACombatSystem>(TEXT("Combat System"));
}

void AAgentController::AgentBehaviorChoosing(AAgent* CallerAgent)
{
	if (!CallerAgent || !WanderingSystem || !FoodSearchSystem || !CombatSystem) return;
	else {
		EAnimalState State = CallerAgent->Attributes->GetAgentState();
		AAgent* Enemy = CombatSystem->DetectEnemyInRange(CallerAgent);
		AEnvironmentActor* Food = FoodSearchSystem->DetectFoodInRange(CallerAgent);
		if (Food) {
			FString FoodName = Food->GetName();
			PRINT_STRING(FoodName)
		}
		if (Enemy) {
			FString EnemyName = Enemy->GetName();
			PRINT_STRING(EnemyName)
		}
		switch (State) {
		case EAnimalState::EAS_Sleeping:
			if (!Enemy) {
				GetUpOrSleep(CallerAgent);
				break;
			}
			else {
				PossessAndGetUp(CallerAgent);
				break;
			}
		case EAnimalState::EAS_Active:
			if (Enemy) {
				ReactToEnemy(CallerAgent, Enemy);
				break;
			}
			else if (!Enemy && Food) {
				PossessAndSearchFood(CallerAgent, Food);
				break;
			}
			else {
				PossessAndMoveInTerritory(CallerAgent);
				break;
			}
		case EAnimalState::EAS_Hunting:
			if (Enemy) {
				ReactToEnemy(CallerAgent, Enemy);
				break;
			}
			else if (!Enemy && Food) {
				PossessAndSearchFood(CallerAgent, Food);
				break;
			}
			else {
				PossessAndMoveInTerritory(CallerAgent);
				break;
			}
		case EAnimalState::EAS_Chasing:
			if (Enemy) {
				ReactToEnemy(CallerAgent, Enemy);
				break;
			}
			else break;
		case EAnimalState::EAS_Eating:
			if (Enemy) {
				ReactToEnemy(CallerAgent, Enemy);
				break;
			}
			else {
				//if Health too low, sleep at that spot otherwise move to random location;
				if (bHealthEnergyCheck(CallerAgent)) return;
				else PossessAndMoveInTerritory(CallerAgent);
				break;
			}
		}

	}
}

void AAgentController::PossessAndGetUp(AAgent* CallerAgent)
{
	if (!WanderingSystem) return;
	WanderingSystem->Possess(CallerAgent);
	WanderingSystem->GetUp(CallerAgent);
	this->Possess(CallerAgent);
}

void AAgentController::PossessAndSearchFood(AAgent* CallerAgent, AEnvironmentActor* Food)
{
	if (!FoodSearchSystem) return;
	FoodSearchSystem->Possess(CallerAgent);
	FoodSearchSystem->GoToFood(CallerAgent, Food);
	this->Possess(CallerAgent);
}

void AAgentController::PossessAndMoveInTerritory(AAgent* CallerAgent)
{
	if (!WanderingSystem) return;
	WanderingSystem->Possess(CallerAgent);
	WanderingSystem->MoveInTerritory(CallerAgent);
	this->Possess(CallerAgent);
}

void AAgentController::GetUpOrSleep(AAgent* CallerAgent)
{
	if (!CallerAgent && !CallerAgent->Attributes) return;
	float Energy = CallerAgent->Attributes->Energy;
	float Health = CallerAgent->Attributes->AgentHealth;
	float MaxHealth = CallerAgent->Attributes->MaxAgentHealth;
	if (Energy > 45.f) {
		FString AgentGetUp = CallerAgent->GetName() + TEXT("Getting up");
		PRINT_STRING_BLUE(AgentGetUp)
		CallerAgent->Attributes->SetAgentState(EAnimalState::EAS_Active);
	}
	else {
		FString AgentResting = CallerAgent->GetName() + TEXT("Resting");
		PRINT_STRING_BLUE(AgentResting)
		RegenEnergyAndHealth(Health, MaxHealth, Energy);
		
		
	}
	return;
}

void AAgentController::RegenEnergyAndHealth(float& Health, float MaxHealth, float& Energy)
{
	if (Health == MaxHealth) {
		Energy += FMath::RandRange(2.f, 2.5f);
	}
	else {
		Energy += FMath::RandRange(2.f, 2.5f);
		Health += FMath::RandRange(3.f, 5.f);
	}
}

bool AAgentController::bHealthEnergyCheck(AAgent* CallerAgent)
{
	if (!CallerAgent && !CallerAgent->Attributes) return false;
	float Energy = CallerAgent->Attributes->Energy;
	float Health = CallerAgent->Attributes->AgentHealth;
	float MaxHealth = CallerAgent->Attributes->MaxAgentHealth;
	if (Energy < 45.f || Health < MaxHealth / 2) {
		CallerAgent->Attributes->SetAgentState(EAnimalState::EAS_Sleeping);
		return true;
	}
	else {
		return false;
	}
}

double AAgentController::GetDistance(AAgent* CallerAgent, AAgent* Enemy)
{
	if (!CallerAgent || !Enemy) return 0.0f;
	FVector AgentLocation = CallerAgent->GetActorLocation();
	FVector EnemyLocation = Enemy->GetActorLocation();
	double Distance = UKismetMathLibrary::Vector_Distance(AgentLocation, EnemyLocation);
	return Distance;
}


void AAgentController::ReactToEnemy(AAgent* CallerAgent, AAgent* Enemy)
{
	if (!CallerAgent && !CombatSystem && !Enemy) return;
	CombatSystem->Possess(CallerAgent);
	CombatSystem->Possess(Enemy);
	double Distance = GetDistance(CallerAgent, Enemy);
	PRINT_STRING(TEXT("Reacting to Enemy !"))
	if (CombatSystem->IsPrey(CallerAgent, Enemy)) {
		if (Distance < CallerAgent->CombatRadius) CombatSystem->Attack(CallerAgent, Enemy);
		else {
			PRINT_STRING(TEXT("Chase!"))
			CombatSystem->Chase(CallerAgent, Enemy);
		}
		PossessAgentAndEnemy(CallerAgent, Enemy);
	}
	else if (CombatSystem->IsPredator(CallerAgent, Enemy)) {
		if (Distance < CallerAgent->CombatRadius) CombatSystem->Attack(CallerAgent, Enemy);
		else {
			PRINT_STRING(TEXT("Run Away!"))
			CombatSystem->RunAway(CallerAgent, Enemy);
		}
		PossessAgentAndEnemy(CallerAgent, Enemy);
	}
	else {
		PossessAgentAndEnemy(CallerAgent, Enemy);
		return;
	}
}

void AAgentController::PossessAgentAndEnemy(AAgent* CallerAgent, AAgent* Enemy)
{
	if (!this) return;
	this->Possess(CallerAgent);
	this->Possess(Enemy);
}

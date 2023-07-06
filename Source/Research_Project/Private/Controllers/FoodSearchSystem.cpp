// Fill out your copyright notice in the Description page of Project Settings.


#include "Controllers/FoodSearchSystem.h"
#include "Agents/AgentsHeader.h"
#include "Environment/EnvironmentActor.h"
#include "Environment/EnvironmentAttribute.h"

AFoodSearchSystem::AFoodSearchSystem()
{
}

AEnvironmentActor* AFoodSearchSystem::DetectFoodInRange(AAgent* CallerAgent)
{
	if (!CallerAgent) return nullptr;

	AEnvironmentActor* ClosestFood = nullptr;
	float ClosestDistance = TNumericLimits<float>::Max();
	for (AEnvironmentActor* Food : CallerAgent->GetPossibleFood()) {
		if (!Food) return ClosestFood;
		float Distance = FVector::Distance(CallerAgent->GetActorLocation(), Food->GetActorLocation());
		if (Distance < CallerAgent->Attributes->SightRadius && Distance < ClosestDistance) {
			ClosestFood = Food;
			ClosestDistance = Distance;
		}
	}
	return ClosestFood;
}

void AFoodSearchSystem::GoToFood(AAgent* CallerAgent, AEnvironmentActor* Food)
{
	if (!HasFood(Food)) return;
	else {
		if (!CallerAgent->GetFood().Contains(Food->ActorAttributes->GetType())) {
			PRINT_STRING(TEXT("Moving to food!"))
			MoveToTarget(CallerAgent, Food);
			if (CallerAgent->CheckOverlap(Food)) {
				EatFood(CallerAgent, Food);
			}
		}
	}

}

void AFoodSearchSystem::EatFood(AAgent* CallerAgent, AEnvironmentActor* Food)
{
	if (!HasFood(Food) || !CallerAgent || !CallerAgent->Attributes) return;
	CallerAgent->Attributes->SetAgentState(EAnimalState::EAS_Eating);
	//these values depend on the food but for now, they are thrown like this
	PRINT_STRING(TEXT("Eating food!"))
	CallerAgent->Attributes->Energy *= 1.5;
	CallerAgent->Attributes->AgentHealth += 25;
	Food->StartRenewal();
}

bool AFoodSearchSystem::HasFood(AEnvironmentActor* Food)
{
	if (!Food) return false;
	else return true;
}

void AFoodSearchSystem::MoveToTarget(AAgent* CallerAgent, AEnvironmentActor* Food)
{
	if (this == nullptr || Food == nullptr || CallerAgent == nullptr) return;
	FVector FoodLocation = Food->GetActorLocation();
	CallerAgent->SetActorLocation(FoodLocation);
	LoseEnergy(CallerAgent);
}

void AFoodSearchSystem::LoseEnergy(AAgent* CallerAgent)
{
	float RandFloat = FMath::RandRange(2.5f, 4.f);
	CallerAgent->Attributes->Energy -= RandFloat;
}

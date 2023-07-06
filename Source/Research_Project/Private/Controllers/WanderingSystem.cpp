// Fill out your copyright notice in the Description page of Project Settings.


#include "Controllers/WanderingSystem.h"
#include "Agents/AgentsHeader.h"
#include "NavigationSystem.h"

AWanderingSystem::AWanderingSystem()
{

}

void AWanderingSystem::Sleep(AAgent* CallerAgent)
{
	if (!CallerAgent) return;
	if (CallerAgent->IsSleeping()) return;
	if (CallerAgent->Attributes) {
		CallerAgent->Attributes->SetAgentState(EAnimalState::EAS_Sleeping);
		//play sleep anim;
	}

}

void AWanderingSystem::GetUp(AAgent* CallerAgent)
{
	if (!CallerAgent) return;
	if (CallerAgent->IsActive()) return;
	if (CallerAgent->Attributes) {
		CallerAgent->Attributes->SetAgentState(EAnimalState::EAS_Active);
		//play get up anim;
	}
}

FVector AWanderingSystem::CalculateNavigableLocationInRadius(AAgent* CallerAgent)
{
	if (!CallerAgent) return FVector(0.0f, 0.0f, 0.0f);
	else {
		FVector StartLocation = CallerAgent->GetActorLocation();
		if (UNavigationSystemBase* NavSys = GetWorld()->GetNavigationSystem()) {
			UNavigationSystemV1* NavSyst = Cast<UNavigationSystemV1>(NavSys);
			if(!NavSyst) return FVector(0.0f, 0.0f, 0.0f);
			else {
				FVector NewLocation = NavSyst->GetRandomPointInNavigableRadius(GetWorld(), StartLocation, 500.0f); //as to be changed by navigeable radius
				NewLocation.Z = StartLocation.Z;
				return NewLocation;
			}
		}
		return FVector(0.0f, 0.0f, 0.0f);
	}
}

void AWanderingSystem::MoveInTerritory(AAgent* CallerAgent)
{
	if (!CallerAgent && !CallerAgent->Attributes) return;
	FVector OldLoc = CallerAgent->GetActorLocation();
	FVector NewLoc = CalculateNavigableLocationInRadius(CallerAgent);
	CallerAgent->Attributes->SetAgentState(EAnimalState::EAS_Active);
	CallerAgent->SetActorLocation(NewLoc);
	LoseEnergy(CallerAgent);
}

void AWanderingSystem::LoseEnergy(AAgent* CallerAgent)
{
	float RandFloat = FMath::RandRange(1.5f, 2.5f);
	CallerAgent->Attributes->Energy -= RandFloat;
}

void AWanderingSystem::MoveToCertainLocation(FVector Location)
{
	if (this == nullptr) return;
	FAIMoveRequest MoveRequest; 
	PRINT_STRING(TEXT("Move to location"))
	MoveRequest.SetGoalLocation(Location);
	MoveRequest.SetAcceptanceRadius(20.f);
	MoveToLocation(Location, 20.0f);
	PRINT_STRING(TEXT("Hello there"))
}

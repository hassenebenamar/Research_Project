// Fill out your copyright notice in the Description page of Project Settings.


#include "Controllers/CombatSystem.h"
#include "Agents/AgentsHeader.h"

ACombatSystem::ACombatSystem()
{
}

AAgent* ACombatSystem::DetectEnemyInRange(AAgent* CallerAgent)
{
	if (!CallerAgent) return nullptr;

	AAgent* ClosestEnemy = nullptr;
	float ClosestDistance = TNumericLimits<float>::Max();
	for (AAgent* Enemy : CallerAgent->GetPossibleEnemies()) {
		if (!Enemy) return ClosestEnemy;
		if (Enemy->Attributes->bIsDead() || Enemy->Attributes->AgentSpecie == CallerAgent->Attributes->AgentSpecie) {
			continue;
		}
		float Distance = FVector::Distance(CallerAgent->GetActorLocation(), Enemy->GetActorLocation());
		if (Distance < CallerAgent->Attributes->SightRadius && Distance < ClosestDistance) {
			ClosestEnemy = Enemy;
			ClosestDistance = Distance;
		}
	}
	return ClosestEnemy;

}

void ACombatSystem::Chase(AAgent* CallerAgent, AAgent* Enemy)
{
	if (!HasEnemy(Enemy)) return;
	else {
		if (!CallerAgent->Attributes && !Enemy->Attributes) return;
		CallerAgent->Attributes->SetAgentState(EAnimalState::EAS_Chasing);
		MoveToTarget(Enemy);
		LoseEnergy(CallerAgent);
	}
}

void ACombatSystem::LoseEnergy(AAgent* CallerAgent)
{
	float RandFloat = FMath::RandRange(4.f, 6.f);
	CallerAgent->Attributes->Energy -= RandFloat;
}

void ACombatSystem::RunAway(AAgent* CallerAgent, AAgent* Enemy)
{
	if (!HasEnemy(Enemy)) return;
	else {
		if (!CallerAgent->Attributes && !Enemy->Attributes) return;
		CallerAgent->Attributes->SetAgentState(EAnimalState::EAS_Active);
		SelectRunAwayLocation(CallerAgent, Enemy);
		LoseEnergy(CallerAgent);
	}
	
}

void ACombatSystem::Attack(AAgent* CallerAgent, AAgent* Enemy)
{
	if (!HasEnemy(Enemy)) return;
	else {
		if (!CallerAgent->Attributes && !Enemy->Attributes) return;
		CallerAgent->Attributes->SetAgentState(EAnimalState::EAS_Attacking);
		Enemy->Attributes->SetAgentState(EAnimalState::EAS_Attacking);
		HandleDamage(CallerAgent, Enemy);
		LoseEnergy(CallerAgent);
		LoseEnergy(Enemy);
	}
}

bool ACombatSystem::HasEnemy(AAgent* Enemy)
{
	if (!Enemy) return false;
	else return true;
}

bool ACombatSystem::IsPrey(AAgent* CallerAgent, AAgent* Enemy)
{
	return CallerAgent->GetPrey().Contains(Enemy->Attributes->AgentSpecie);
}

bool ACombatSystem::IsPredator(AAgent* CallerAgent, AAgent* Enemy)
{
	return CallerAgent->GetPredators().Contains(Enemy->Attributes->AgentSpecie);
}

void ACombatSystem::SelectRunAwayLocation(AAgent* CallerAgent, AAgent* Enemy)
{
	if (!this || !CallerAgent || !Enemy) return;
	FVector NewAgentLocation = CalculateRunAwayLocation(CallerAgent, Enemy);
	CallerAgent->SetActorLocation(NewAgentLocation);
}

FVector ACombatSystem::CalculateRunAwayLocation(AAgent* CallerAgent, AAgent* Enemy)
{
	FVector AgentLocation = CallerAgent->GetActorLocation();
	FVector EnemyLocation = Enemy->GetActorLocation();
	FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(AgentLocation, EnemyLocation);
	FVector ForwardVector = UKismetMathLibrary::GetForwardVector(Rotation);
	ForwardVector = ForwardVector * -500.f;
	FVector NewAgentLocation = AgentLocation + ForwardVector;
	NewAgentLocation.Z = AgentLocation.Z;
	return NewAgentLocation;
}

void ACombatSystem::HandleDamage(AAgent* Agent, AAgent* Enemy)
{
	if (!Agent) return;
	if (Enemy && Enemy->Attributes) {
		Enemy->Attributes->AgentHealth -= Agent->Attributes->AgentDamage;
		if (Enemy->Attributes->bIsDead()) Enemy->Die();
		else {
			Enemy->Attributes->AgentHealth -= Enemy->Attributes->AgentDamage;
			if (Agent->Attributes->bIsDead()) Agent->Die();
		}
	}
	else {
		if (Agent->Attributes->bIsDead()) Agent->Die();
	}
}



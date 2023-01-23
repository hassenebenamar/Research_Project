// Fill out your copyright notice in the Description page of Project Settings.

#include "Agents/AgentAttribute.h"
#include "Kismet/KismetMathLibrary.h"
#include "Research_Project/DebugMacros.h"

UAgentAttribute::UAgentAttribute()
{
	PrimaryComponentTick.bCanEverTick = true;
	AgentHealth = 100.f;
	AgentSpeed = 300.f;
	
}

void UAgentAttribute::BeginPlay()
{
	Super::BeginPlay();

}

void UAgentAttribute::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

float UAgentAttribute::CalculateDamage(float Size)
{
	const float Damage = (0.3f * Size);
	return Damage;
}

float UAgentAttribute::CalculateHealth(float Size, float Damage)
{
	const float Health = (0.3f * Size + 1.5f * Damage);
	return Health;
}

float UAgentAttribute::CalculateSize(float CapsuleHalfRadius)
{
	const float Size = (4.f / 3.f) * FMath::Cube(CapsuleHalfRadius) * UKismetMathLibrary::GetPI();
	return Size;
}

float UAgentAttribute::CalculateSurvivability(float Speed, float Damage, float Size, float Health)
{
	const float exp_speed = FMath::Exp(Speed);
	const float log_size = FMath::LogX(10.f, Size);
	const float log_health = FMath::Log2(Health);
	const float Survivability = (1.5f * exp_speed + 3.f * (log_size + log_health) + 3.5f * Damage);
	return Survivability;
}

bool UAgentAttribute::bIsAlive()	
{
	return AgentHealth > 0.f;
}

bool UAgentAttribute::bIsDead()
{
	return AgentHealth == 0.f;
}

bool UAgentAttribute::bIsHunting()
{
	return AnimalState == EAnimalState::EAS_Hunting;
}

bool UAgentAttribute::bIsAttacking()
{
	return AnimalState == EAnimalState::EAS_Attacking;
}

bool UAgentAttribute::bIsActive()
{
	return AnimalState == EAnimalState::EAS_Active;
}

bool UAgentAttribute::bIsSleeping()
{
	return AnimalState == EAnimalState::EAS_Sleeping;
}

bool UAgentAttribute::bIsEnvIdeal()
{
	return EnvironmentType == IdealEnvironment;
}

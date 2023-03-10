// Fill out your copyright notice in the Description page of Project Settings.

#include "Agents/AgentAttribute.h"
#include "Agents/AgentsHeader.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Research_Project/DebugMacros.h"

UAgentAttribute::UAgentAttribute()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UAgentAttribute::BeginPlay()
{
	Super::BeginPlay();

}

void UAgentAttribute::CalculateBaseStats(AAgent* Agent)
{
	if (!Agent) return;
	TSubclassOf<AAgent> AgentClass = Agent->GetClass();
	FString ClassName = Agent->GetName();
	if (!Agent->Attributes) return;
	Agent->Attributes->Energy = 100;
	CalculateStatPerAS(ClassName, Agent, EFloatPropertyName::EFPN_SightRange);
	CalculateStatPerAS(ClassName, Agent, EFloatPropertyName::EFPN_SightRadius);
	CalculateStatPerAS(ClassName, Agent, EFloatPropertyName::EFPN_SightOffset);
	CalculateStatPerAS(ClassName, Agent, EFloatPropertyName::EFPN_HearingThreshold);
}

void UAgentAttribute::CalculateStatPerAS(FString& ClassName, AAgent* Agent, EFloatPropertyName PropertyEnum)
{
	if (!Agent) return;
	TArray<float> Bonuses = CalculateSenseBonus(Agent, ClassName);
	if (Bonuses.Num() == 0) {
		Bonuses.Add(UKismetMathLibrary::RandomFloat() * 20);
		Bonuses.Add(UKismetMathLibrary::RandomFloat() * 40);
	}
	switch (PropertyEnum) {
	case EFloatPropertyName::EFPN_SightRange:
		if (ClassName.Contains(TEXT("Deer"))) {
			AgentSpecie = EAgentSpecie::EAS_Deer;
			for (uint8 i = 0; i < Agent->Attributes->DNA.Num(); i++) {
				Agent->Attributes->SightRange = Agent->Attributes->SightRange + Agent->Attributes->DNA[i] * 45 + Bonuses[0];
			}
		}
		else if (ClassName.Contains(TEXT("Fox"))) {
			AgentSpecie = EAgentSpecie::EAS_Fox;
			for (uint8 i = 0; i < Agent->Attributes->DNA.Num(); i++) {
				Agent->Attributes->SightRange = Agent->Attributes->SightRange + Agent->Attributes->DNA[i] * 25 + Bonuses[0];
			}
		}
		else {
			Agent->Attributes->SightRange =  UKismetMathLibrary::RandomFloat() * 150;
		}
		break;
	case EFloatPropertyName::EFPN_SightRadius:
		if (ClassName.Contains(TEXT("Deer"))) {
			AgentSpecie = EAgentSpecie::EAS_Deer;
			for (uint8 i = 0; i < Agent->Attributes->DNA.Num(); i++) {
				Agent->Attributes->SightRadius = Agent->Attributes->SightRadius + Agent->Attributes->DNA[i] * 50 + Bonuses[0];
			}
		}
		else if (ClassName.Contains(TEXT("Fox"))) {
			AgentSpecie = EAgentSpecie::EAS_Fox;
			for (uint8 i = 0; i < Agent->Attributes->DNA.Num(); i++) {
				Agent->Attributes->SightRadius = Agent->Attributes->SightRadius + Agent->Attributes->DNA[i] * 30 + Bonuses[0];
			}
		}
		else {
			Agent->Attributes->SightRadius = UKismetMathLibrary::RandomFloat() * 100;
		}
		break;
	case EFloatPropertyName::EFPN_SightOffset:
		if (ClassName.Contains(TEXT("Deer"))) {
			AgentSpecie = EAgentSpecie::EAS_Deer;
			for (uint8 i = 0; i < Agent->Attributes->DNA.Num(); i++) {
				Agent->Attributes->SightOffset = Agent->Attributes->SightOffset + Agent->Attributes->DNA[i] * 5 + Bonuses[0];
			}
		}
		else if (ClassName.Contains(TEXT("Fox"))) {
			AgentSpecie = EAgentSpecie::EAS_Fox;
			for (uint8 i = 0; i < Agent->Attributes->DNA.Num(); i++) {
				Agent->Attributes->SightOffset = Agent->Attributes->SightOffset + Agent->Attributes->DNA[i] * 15 + Bonuses[0];
			}
		}
		else {
			Agent->Attributes->SightOffset = UKismetMathLibrary::RandomFloat() * 100;
		}
		break;
	case EFloatPropertyName::EFPN_HearingThreshold:
		if (ClassName.Contains(TEXT("Deer"))) {
			AgentSpecie = EAgentSpecie::EAS_Deer;
			for (uint8 i = 0; i < Agent->Attributes->DNA.Num(); i++) {
				Agent->Attributes->HearingThreshold = Agent->Attributes->HearingThreshold + Agent->Attributes->DNA[i] * 100 + Bonuses[1];
			}
		}
		else if (ClassName.Contains(TEXT("Fox"))) {
			AgentSpecie = EAgentSpecie::EAS_Fox;
			for (uint8 i = 0; i < Agent->Attributes->DNA.Num(); i++) {
				Agent->Attributes->HearingThreshold = Agent->Attributes->HearingThreshold + Agent->Attributes->DNA[i] * 150 + Bonuses[1];
			}
		}
		else {
			Agent->Attributes->HearingThreshold = UKismetMathLibrary::RandomFloat() * 100;
		}
		break;
	}
	
}

TArray<float> UAgentAttribute::CalculateSenseBonus(AAgent* Agent, FString& ClassName)
{
	TArray<float> Bonuses;
	float HearingBonus = 0.f;
	float SightBonus = 0.f;
	UAISenseConfig_Hearing* AgentHearing = Agent->HearingConfig;
	UAISenseConfig_Sight* AgentSight = Agent->SightConfig;
	if (!AgentHearing && !AgentSight) {
		SightBonus = 0;
		Bonuses.Add(SightBonus);
		HearingBonus = 0;
		Bonuses.Add(HearingBonus);
	}
	if (AgentHearing && !AgentSight) {
		SightBonus = 0;
		Bonuses.Add(SightBonus);
		HearingBonus = UKismetMathLibrary::RandomFloat() * 20;
		Bonuses.Add(HearingBonus);
	}
	if (!AgentHearing && AgentSight) {
		SightBonus = 40;
		Bonuses.Add(SightBonus);
		HearingBonus = 0;
		Bonuses.Add(HearingBonus);
	}
	if (AgentHearing && AgentSight) {
		if (ClassName.Contains(TEXT("Deer"))) {
			SightBonus = UKismetMathLibrary::RandomFloat() * 10;
			Bonuses.Add(SightBonus);
			HearingBonus = UKismetMathLibrary::RandomFloat() * 20;
			Bonuses.Add(HearingBonus);
		}
		else if (ClassName == TEXT("Fox")) {
			SightBonus = UKismetMathLibrary::RandomFloat() * 10;
			Bonuses.Add(SightBonus);
			HearingBonus = UKismetMathLibrary::RandomFloat() * 50;
			Bonuses.Add(HearingBonus);
		}
		else {
			SightBonus = UKismetMathLibrary::RandomFloat() * 10;
			Bonuses.Add(SightBonus);
			HearingBonus = UKismetMathLibrary::RandomFloat() * 20;
			Bonuses.Add(HearingBonus);
		}
	}
	return Bonuses;
}

void UAgentAttribute::InitializeFloatStats(AAgent* Agent)
{
	if (!Agent) return;
	float Size = CalculateSize(Agent);
	if (Size != 0.f) {
		float Speed = CalculateSpeed(Size);
		float Damage = CalculateDamage(Size);
		if (Speed != 0.f && Damage != 0.f) {
			float Health = CalculateHealth(Size, Damage);
			if (Health != 0.f) CalculateSurvivability(Speed, Damage, Size, Health);
		}
	}
}


void UAgentAttribute::InitializeInt64Stats(AAgent* Agent)
{
	if (!Agent && !Agent->Attributes) return;
	int64 Min = 0;
	int64 Max = FMath::RandHelper64(999999999);
	Agent->Attributes->AgentID = FMath::RandRange(Min, Max);
}

void UAgentAttribute::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

float UAgentAttribute::CalculateDamage(float Size)
{
	AgentDamage = (0.3f * Size);
	return AgentDamage;
}

float UAgentAttribute::CalculateHealth(float Size, float Damage)
{
	AgentHealth = (0.3f * Size + 1.5f * Damage);
	return AgentHealth;
}

float UAgentAttribute::CalculateSize(AAgent* AgentToInit)
{
	if (!AgentToInit) return 0.f;
	UCapsuleComponent* Capsule = AgentToInit->GetCapsuleComponent();
	if (!Capsule) return 0.f;
	float CapsuleSize = Capsule->GetScaledCapsuleRadius();
	if (CapsuleSize == 0.f) return 0.f;
	AgentSize = (4.f / 3.f) * FMath::Cube(CapsuleSize) * UKismetMathLibrary::GetPI();
	return AgentSize;
}


float UAgentAttribute::CalculateSpeed(float Size)
{	
	if (!Size) return 0.f;
	AgentSpeed = FMath::RandRange(0.f, (Size / 10)) * 3;
	return AgentSpeed;
}

float UAgentAttribute::CalculateSurvivability(float Speed, float Damage, float Size, float Health)
{
	const float log2_speed = FMath::Log2(Speed);
	const float log_size = FMath::LogX(10.f, Size);
	const float log_health = FMath::Log2(Health);

	float SenseBonus = 0.f;
	if (SightRange > 0.f && SightRadius > 0.f && SightOffset > 0.f && HearingThreshold > 0.f) {
		SenseBonus = (SightRange / SightRadius) * SightOffset * (1.f / HearingThreshold);
	}
	SurvivabilityScore = (1.5f * log2_speed + 3.f * (log_size + log_health) + 3.5f * Damage) + SenseBonus;
	return SurvivabilityScore;
}


void UAgentAttribute::InitializeNewStats(AAgent* AgentToInit)
{
	if (!AgentToInit && AgentToInit->Attributes) return;
	InitializeDNA(AgentToInit);
	InitializeBaseStats(AgentToInit);
	InitializeFloatStats(AgentToInit);
	InitializeInt64Stats(AgentToInit);
}

void UAgentAttribute::InitializeBaseStats(AAgent* AgentToInit)
{	
	if (!AgentToInit) return;
	CalculateBaseStats(AgentToInit);
}

void UAgentAttribute::InitializeDNA(AAgent* AgentToInit)
{
	if (!AgentToInit && !AgentToInit->Attributes) return;
	if (AgentToInit->Attributes->DNA.Num() == 0) {
		for (uint8 i = 0; i < 4; i++) {
			uint8 ChromosomeToAdd = UKismetMathLibrary::RandomInteger(2);
			DNA.Add(ChromosomeToAdd);
		}
	}
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

// Fill out your copyright notice in the Description page of Project Settings.


#include "Agents/Species/Fox.h"
#include "Agents/AgentsHeader.h"
#include "Components/CapsuleComponent.h"
#include "Perception/PawnSensingComponent.h"

AFox::AFox()
{
	USkeletalMeshComponent* AgentMeshComponent = GetMesh();

	if (AgentMeshComponent) {
		static ConstructorHelpers::FObjectFinder<USkeletalMesh> SkeletalMeshAsset(TEXT("/Game/AnimalVarietyPack/Fox/Meshes/SK_Fox.SK_Fox"));
		AgentMeshComponent->SetSkeletalMesh(SkeletalMeshAsset.Object);
	}

	AgentMeshComponent->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	AgentMeshComponent->SetRelativeLocation(FVector(-2.25f, 0.0f, -33.6f));

	if (Capsule) {
		Capsule->SetCapsuleHalfHeight(36.42f);
		Capsule->SetCapsuleRadius(36.42f);
	}

	CombatRadius = 15.0f;

	this->Preys.Add(EAgentSpecie::EAS_Deer);
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "Agents/Species/Deer.h"
#include "Components/CapsuleComponent.h"

ADeer::ADeer()
{
	USkeletalMeshComponent* AgentMeshComponent = GetMesh();

	if (AgentMeshComponent) {
		static ConstructorHelpers::FObjectFinder<USkeletalMesh> SkeletalMeshAsset(TEXT("/Game/AnimalVarietyPack/DeerStagAndDoe/Meshes/SK_DeerDoe.SK_DeerDoe"));
		AgentMeshComponent->SetSkeletalMesh(SkeletalMeshAsset.Object);
	}

	AgentMeshComponent->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	AgentMeshComponent->SetRelativeLocation(FVector(5.5f, 0.0f, -88.87f));

	if (Capsule) {
		Capsule->SetCapsuleHalfHeight(92.97f);
		Capsule->SetCapsuleRadius(88.0f);
	}
}


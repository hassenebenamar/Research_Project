// Fill out your copyright notice in the Description page of Project Settings.


#include "Agents/Species/Deer.h"
#include "Agents/AgentsHeader.h"
#include "Research_Project/DebugMacros.h"
#include "Components/CapsuleComponent.h"
#include "Perception/PawnSensingComponent.h"

ADeer::ADeer()
{
	USkeletalMeshComponent* AgentMeshComponent = GetMesh();

	if (AgentMeshComponent) {
		static ConstructorHelpers::FObjectFinder<USkeletalMesh> SkeletalMeshAsset(TEXT("/Game/AnimalVarietyPack/DeerStagAndDoe/Meshes/SK_DeerDoe.SK_DeerDoe"));
		AgentMeshComponent->SetSkeletalMesh(SkeletalMeshAsset.Object);
	}

	//setting up animations
    if (AnimArray.Num() == 0) {
        static const TArray<FString> AnimPaths = {
            TEXT("/Game/AnimalVarietyPack/DeerStagAndDoe/Animations/ANIM_DeerDoe_IdleSleep.ANIM_DeerDoe_IdleSleep"),
            TEXT("/Game/AnimalVarietyPack/DeerStagAndDoe/Animations/ANIM_DeerDoe_SleepToGoBackUp.ANIM_DeerDoe_SleepToGoBackUp"),
            TEXT("/Game/AnimalVarietyPack/DeerStagAndDoe/Animations/ANIM_DeerDoe_Run.ANIM_DeerDoe_Run"),
            TEXT("/Game/AnimalVarietyPack/DeerStagAndDoe/Animations/ANIM_DeerDoe_IdleLookAround.ANIM_DeerDoe_IdleLookAround"),
            TEXT("/Game/AnimalVarietyPack/DeerStagAndDoe/Animations/ANIM_DeerDoe_IdleChew.ANIM_DeerDoe_IdleChew")
        };

        for (const FString& AnimPath : AnimPaths) {
            static ConstructorHelpers::FObjectFinder<UAnimSequence> AnimObj(*AnimPath);
            UAnimSequence* Anim = AnimObj.Object;
            if (Anim) {
                AnimArray.Add(Anim);
            }
        }
    }

	AgentMeshComponent->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	AgentMeshComponent->SetRelativeLocation(FVector(5.5f, 0.0f, -88.87f));

	if (Capsule) {
		Capsule->SetCapsuleHalfHeight(92.97f);
		Capsule->SetCapsuleRadius(88.0f);
	}

    CombatRadius = 20.0f;
    this->Predators.Add(EAgentSpecie::EAS_Fox);
}


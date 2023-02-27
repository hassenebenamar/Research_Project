// Fill out your copyright notice in the Description page of Project Settings.


#include "Agents/Species/Deer.h"
#include "Components/CapsuleComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionSystem.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"

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

    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Agent Sight Config"));
    HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("Agent Hearing Config"));
    if (World && SightConfig) {
        SightConfig->SightRadius = 1000.f;
        SightConfig->LoseSightRadius = 1200.f;
        SightConfig->PeripheralVisionAngleDegrees = 40.0f;
        SightConfig->DetectionByAffiliation.bDetectEnemies = true;
        SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
        SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

        // Register the sight sensing component with the AI perception system
        UAIPerceptionSystem::RegisterPerceptionStimuliSource(World, SightConfig->GetSenseImplementation(), this);
    }
    if (World && HearingConfig) {
        HearingConfig->HearingRange = 400.f;
        HearingConfig->LoSHearingRange = 500.f;

        // Register the sight sensing component with the AI perception system
        UAIPerceptionSystem::RegisterPerceptionStimuliSource(World, HearingConfig->GetSenseImplementation(), this);
    }

    PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Agent Perception Component"));
    if (PerceptionComponent) {
        PerceptionComponent->ConfigureSense(*SightConfig);
        PerceptionComponent->ConfigureSense(*HearingConfig);
        PerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());
    }
}


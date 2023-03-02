// Fill out your copyright notice in the Description page of Project Settings.


#include "Agents/Agent.h"
#include "Agents/AgentAttribute.h"
#include "Environment/EnvironmentActor.h"
#include "Research_Project/DebugMacros.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Components/AudioComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionSystem.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/PawnSensingComponent.h"
#include "MetasoundSource.h"
#include "Sound/SoundWave.h"
#include "NavigationSystem.h"

AAgent::AAgent() {

	PrimaryActorTick.bCanEverTick = true;

	Attributes = CreateDefaultSubobject<UAgentAttribute>(TEXT("Attributes"));
}

void AAgent::CopyObject(const AAgent& ObjectToCopy)
{
	{	
		if (this == nullptr) return;
		if (!ObjectToCopy.IsValidLowLevel()) return;

		// Get the class of the object to copy
		UClass* ClassToCopy = ObjectToCopy.GetClass();

		if (ClassToCopy == nullptr) return;

		// Loop through all properties of the class and copy their values
		for (TFieldIterator<FProperty> PropIt(ClassToCopy); PropIt; ++PropIt)
		{
			FProperty* Property = *PropIt;
			
			if (Property == nullptr) return;

			// Ignore properties that are not editable or not replicated
			if (!Property->HasAnyPropertyFlags(CPF_Edit | CPF_Net))
			{
				continue;
			}

			// Get the value of the property on the other object
			const uint8* ValuePtr = Property->ContainerPtrToValuePtr<uint8>(&ObjectToCopy);

			if (ValuePtr == nullptr) return;

			// Set the value of the property on this object
			Property->CopyCompleteValue(Property->ContainerPtrToValuePtr<uint8>(this), ValuePtr);
		}
	}
}

void AAgent::BeginPlay()
{
	Super::BeginPlay();
	World = GetWorld();
	if (Attributes) {
		Attributes->InitializeNewStats(this);
	}
	AgentController = Cast<AAIController>(GetController());
	OnActorBeginOverlap.AddDynamic(this, &AAgent::OnActorOverlap);
}

//status checks

bool AAgent::IsAlive()
{
	return Attributes && Attributes->bIsAlive();
}

bool AAgent::IsDead()
{
	return Attributes && Attributes->bIsDead();

}

bool AAgent::IsActive()
{
	return Attributes && Attributes->bIsActive();
}

bool AAgent::IsHunting()
{
	return Attributes && Attributes->bIsHunting();
}

bool AAgent::IsAttacking()
{
	return Attributes && Attributes->bIsAttacking();
}

bool AAgent::IsSleeping()
{
	return Attributes && Attributes->bIsSleeping();
}

float AAgent::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	return 0.0f;
}

bool AAgent::IsInCombatRange(AAgent* Target)
{
	return GetActorLocation().X - Enemy->GetActorLocation().X >= CombatRadius;
}

void AAgent::HostileAgentDetected(AActor* DetectedAgent, FAIStimulus Stimulus)
{
	TSubclassOf<UAISense> Sense = UAIPerceptionSystem::GetSenseClassForStimulus(this, Stimulus);
	if (Sense) {
		Enemy = Cast<AAgent>(DetectedAgent);
		if (Enemy) {
			ReactToEnemy();
			if (IsInCombatRange(Enemy)) Attack();
		}
	}
}

void AAgent::ReactToEnemy()
{
	if (Attributes && AgentController && Enemy && Enemy->Attributes) {
		Attributes->AnimalState = EAnimalState::EAS_Active;
		EAgentSpecie AgentSpecie = Enemy->Attributes->AgentSpecie;
		if (Preys.Contains(AgentSpecie)) {
			MoveToTarget(Enemy);
		}
		if (Predators.Contains(AgentSpecie)) {
			RunAway();
		}
	}
}

bool AAgent::SphereTrace()
{
	if (!World || !Attributes) return false;
	if (World && Attributes) {
		FVector StartLocation = GetActorLocation() + FVector(0.f, 0.f, Attributes->SightOffset);
		FVector EndLocation = GetActorLocation() + Attributes->SightRange;
		float Radius = Attributes->SightRadius * 0.5f;
		TArray<FHitResult> HitResults;
		FCollisionQueryParams QueryParams;
		bool bIsHit = World->SweepMultiByChannel(HitResults,StartLocation,EndLocation,FQuat::Identity,ECC_Visibility,FCollisionShape::MakeSphere(Radius),QueryParams);
		if (!bIsHit) return false;
		for (FHitResult& HitResult : HitResults) {
			AEnvironmentActor* HitTarget = Cast<AEnvironmentActor>(HitResult.GetActor());
			if (HitTarget) {
				GreenTargets.Add(HitTarget);
			}
		}
	}
	return true;
}

void AAgent::ReactToGreenTarget()
{
	ClearGreenTargetTimer();
	if(!SphereTrace()) return;
	if (GreenTargets.Num() > 0) {
		AEnvironmentActor* ClosestGreenTarget = nullptr;
		float ClosestDistance = TNumericLimits<float>::Max();
		for (AEnvironmentActor* GreenTarget : GreenTargets) {
			float Distance = FVector::Distance(GetActorLocation(), GreenTarget->GetActorLocation());
			if (Distance < ClosestDistance) {
				ClosestGreenTarget = GreenTarget;
				ClosestDistance = Distance;
			}
		}
		if (ClosestGreenTarget) {
			MoveToTarget(ClosestGreenTarget);
		}
	}
}

void AAgent::Sleep()
{
	if (IsSleeping()) return;
	if (Attributes) {
		Attributes->AnimalState = EAnimalState::EAS_Sleeping;
		PlaySleepAnim();
	}
	
}

void AAgent::GetUp()
{
	if (IsActive()) return;
	if (Attributes) {
		Attributes->AnimalState = EAnimalState::EAS_Active;
		PlayGetUpAnim();
	}
}

void AAgent::Hunt()
{
	if (IsHunting()) return;
	if (Attributes) {
		Attributes->AnimalState = EAnimalState::EAS_Hunting;
		PlayHuntingAnim();
	}
}

void AAgent::Attack()
{
	if (IsAttacking()) return;
	if (Attributes) {
		ClearAttackTimer();
		Attributes->AnimalState = EAnimalState::EAS_Attacking;
		StartAttackTimer();
		PlayAttackMontage();
		Attributes->AnimalState = EAnimalState::EAS_Active;
	}
}

void AAgent::RunAway()
{
	if (!IsActive()) return;
	if (Attributes) {
		GetRunAwayLocation();
		PlayRunAwayAnim();
	}
}

void AAgent::GetRunAwayLocation()
{
	if (AgentController) {
		FVector AgentLocation = GetActorLocation();
		FVector EnemyLocation = Enemy->GetActorLocation();
		FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(AgentLocation, EnemyLocation);
		FVector ForwardVector = UKismetMathLibrary::GetForwardVector(Rotation);
		ForwardVector = ForwardVector * -500.f;
		FVector NewAgentLocation = AgentLocation + ForwardVector;
		NewAgentLocation.Z = AgentLocation.Z;
		//NavigationSystem->GetRandomPointInNavigableRadius(this, NewAgentLocation, 50.f);
		FAIMoveRequest MoveRequest;
		MoveRequest.SetGoalLocation(NewAgentLocation);
		AgentController->MoveTo(MoveRequest);
	}
}

void AAgent::Die()
{
	if (IsDead()) return;
	if (Attributes) {
		Attributes->AnimalState = EAnimalState::EAS_Dead;
		PlayDeathMontage();
	}
}

void AAgent::StartAttackTimer()
{
	GetWorldTimerManager().SetTimer(AttackTimer, this, &AAgent::Attack, AttackTime);
}

void AAgent::ClearAttackTimer()
{
	GetWorldTimerManager().ClearTimer(AttackTimer);
}

void AAgent::StartGreenTargetTimer()
{
	GetWorldTimerManager().SetTimer(GreenTargetTimerHandler, this, &AAgent::ReactToGreenTarget, 1.f);
}

void AAgent::ClearGreenTargetTimer()
{
	GetWorldTimerManager().ClearTimer(GreenTargetTimerHandler);
}

void AAgent::OnActorOverlap(AActor* OverlappedActor, AActor* OtherActor)
{	
	OverlappedEnvActor = Cast<AEnvironmentActor>(OverlappedActor);
	if (OverlappedEnvActor) {
		//PlayEatingAnim();
		OverlappedEnvActor->StartRenewal();
	}
}


void AAgent::PlaySleepAnim()
{	
	USkeletalMeshComponent* AgentMeshComponent = GetMesh();
	if (AgentMeshComponent && AnimArray.Num() != 0) {
		AgentMeshComponent->SetAnimation(AnimArray[0]);
		AgentMeshComponent->Play(true);
	}
}

void AAgent::PlayGetUpAnim()
{
	USkeletalMeshComponent* AgentMeshComponent = GetMesh();
	if (AgentMeshComponent && AnimArray.Num() != 0) {
		AgentMeshComponent->SetAnimation(AnimArray[1]);
		AgentMeshComponent->Play(false);
	}
}

void AAgent::PlayRunAwayAnim()
{
	USkeletalMeshComponent* AgentMeshComponent = GetMesh();
	if (AgentMeshComponent && AnimArray.Num() != 0) {
		AgentMeshComponent->SetAnimation(AnimArray[2]);
		AgentMeshComponent->Play(true);
	}
}

void AAgent::PlayHuntingAnim()
{
	USkeletalMeshComponent* AgentMeshComponent = GetMesh();
	if (AgentMeshComponent && AnimArray.Num() != 0) {
		AgentMeshComponent->SetAnimation(AnimArray[3]);
		AgentMeshComponent->Play(true);
	}
}

void AAgent::PlayEatingAnim()
{
	USkeletalMeshComponent* AgentMeshComponent = GetMesh();
	if (AgentMeshComponent && AnimArray.Num() != 0) {
		AgentMeshComponent->SetAnimation(AnimArray[4]);
		AgentMeshComponent->Play(true);
	}
}

void AAgent::PlayAttackMontage()
{
}

void AAgent::PlayDeathMontage()
{
}


void AAgent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AAgent::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


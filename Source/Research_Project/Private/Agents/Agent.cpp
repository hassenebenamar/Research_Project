// Fill out your copyright notice in the Description page of Project Settings.


#include "Agents/Agent.h"
#include "Agents/AgentAttribute.h"
#include "Environment/EnvironmentActor.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Components/AudioComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Controllers/AgentController.h"
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
		//PRINT_STRING(TEXT("Setting attributes, begin play"))
		Attributes->SetAgentState(EAnimalState::EAS_Active);
	}
	StartBehaviorTimer();
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

TArray<AEnvironmentActor*> AAgent::GetPossibleFood()
{
	if (!World || !Attributes) return GreenTargets;
	if (World && Attributes) {
		FVector StartLocation = GetActorLocation();
		StartLocation.Z = 0.f;
		FVector EndLocation = GetActorLocation() + (GetActorForwardVector() * Attributes->SightRange);
		EndLocation.Z = GetActorLocation().Z + Attributes->SightOffset * 0.5f;
		float Radius = Attributes->SightRadius;
		TArray<FHitResult> HitResults;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);
		bool bIsHit = World->SweepMultiByChannel(HitResults, StartLocation, EndLocation, FQuat::Identity, ECC_GameTraceChannel2, FCollisionShape::MakeSphere(Radius), QueryParams);
		if (!bIsHit) return GreenTargets;
		for (FHitResult& HitResult : HitResults) {
			AEnvironmentActor* HitActor = Cast<AEnvironmentActor>(HitResult.GetActor());
			if (HitActor) {
				GreenTargets.Add(HitActor);
			}
		}
		return GreenTargets;
	}
	return GreenTargets;
}

TArray<AAgent*> AAgent::GetPossibleEnemies()
{
	if (!World || !Attributes) return Targets;
	if (World && Attributes) {
		FVector StartLocation = GetActorLocation();
		StartLocation.Z = 0.f;
		FVector EndLocation = GetActorLocation() + (GetActorForwardVector() * Attributes->SightRange);
		EndLocation.Z = GetActorLocation().Z + Attributes->SightOffset * 0.5f;
		float Radius = Attributes->SightRadius;
		TArray<FHitResult> HitResults;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);
		bool bIsHit = World->SweepMultiByChannel(HitResults, StartLocation, EndLocation, FQuat::Identity, ECC_Pawn, FCollisionShape::MakeSphere(Radius), QueryParams);
		if (!bIsHit) return Targets;
		for (FHitResult& HitResult : HitResults) {
			AAgent* HitAgent = Cast<AAgent>(HitResult.GetActor());
			if (HitAgent) {
				Targets.Add(HitAgent);
			}
		}
		return Targets;
	}
	return Targets;
}

void AAgent::Die()
{
	if (IsDead()) return;
	if (Attributes) {
		PRINT_STRING(TEXT("Agent is dead"))
		Attributes->AnimalState = EAnimalState::EAS_Dead;
		Destroy();
	}
}

bool AAgent::CheckOverlap(AActor* OverlapActor)
{	
	if (!this || !OverlapActor)
	{
		return false;
	}

	FCollisionQueryParams CollisionParams;
	FCollisionShape CollisionShape;
	CollisionShape.ShapeType = ECollisionShape::Sphere;
	CollisionShape.SetSphere(40.f);

	const bool bOverlap = this->GetWorld()->OverlapAnyTestByChannel(
		GetActorLocation(),
		FQuat::Identity,
		ECC_GameTraceChannel2,
		CollisionShape,
		CollisionParams);

	if (bOverlap)
	{
		return true;
	}

	return false;
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

void AAgent::CallBehaviorTree()
{
	AAgentController* BehaviorSystem = NewObject<AAgentController>(this);
	if (BehaviorSystem) {
		//PRINT_STRING(TEXT("Running Behavior Tree"))
		BehaviorSystem->AgentBehaviorChoosing(this);
	}
	ClearBehaviorTimer();
	StartBehaviorTimer();
}

void AAgent::StartBehaviorTimer()
{
	//PRINT_STRING(TEXT("Starting behavior timer"))
	GetWorldTimerManager().SetTimer(BehaviorTimer, this, &AAgent::CallBehaviorTree, 3.f);
}

void AAgent::ClearBehaviorTimer()
{
	GetWorldTimerManager().ClearTimer(BehaviorTimer);
}


void AAgent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AAgent::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


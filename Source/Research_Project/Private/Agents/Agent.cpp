// Fill out your copyright notice in the Description page of Project Settings.


#include "Agents/Agent.h"
#include "Agents/AgentAttribute.h"
#include "Environment/EnvironmentActor.h"
#include "Research_Project/DebugMacros.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionSystem.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/PawnSensingComponent.h"
#include "NavigationSystem.h"

AAgent::AAgent() {

	PrimaryActorTick.bCanEverTick = true;

	Attributes = CreateDefaultSubobject<UAgentAttribute>(TEXT("Attributes"));
}

void AAgent::BeginPlay()
{
	Super::BeginPlay();
	World = GetWorld();
	UCapsuleComponent* Capsule = ACharacter::GetCapsuleComponent();
	if (Attributes && Capsule) {
		float AgentSize = Attributes->CalculateSize(Capsule->GetScaledCapsuleRadius());
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

void AAgent::ActorSeen(AActor* SeenActor, FAIStimulus Stimulus)
{
	TSubclassOf<UAISense> Sense = UAIPerceptionSystem::GetSenseClassForStimulus(this, Stimulus);
	if (Sense) {
		Enemy = Cast<AAgent>(SeenActor);
		if (Enemy) {
			ReactToEnemy();
			if (IsInCombatRange(Enemy)) Attack();
		}
	}
}

void AAgent::ReactToEnemy()
{
	if (Attributes && AgentController && Attributes) {
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
		PRINT_STRING(TEXT("Getting enemy location"))
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
	USkeletalMeshComponent* AgentMesh = GetMesh();
	if (AgentMesh && SleepAnim) {
		AgentMesh->SetAnimationMode(EAnimationMode::AnimationSingleNode);
		AgentMesh->SetAnimation(SleepAnim);
		AgentMesh->Play(true);
	}
}

void AAgent::PlayGetUpAnim()
{
}

void AAgent::PlayRunAwayAnim()
{
}

void AAgent::PlayHuntingAnim()
{
}

void AAgent::PlayEatingAnim()
{
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


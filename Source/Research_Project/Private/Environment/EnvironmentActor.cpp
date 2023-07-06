// Fill out your copyright notice in the Description page of Project Settings.


#include "Environment/EnvironmentActor.h"
#include "Environment/EnvironmentAttribute.h"
#include "Research_Project/DebugMacros.h"
#include "Components/CapsuleComponent.h"

AEnvironmentActor::AEnvironmentActor()
{
	PrimaryActorTick.bCanEverTick = true;

	EnvironmentMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	SetRootComponent(EnvironmentMesh);
	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	Capsule->SetupAttachment(GetRootComponent());

	ActorAttributes = CreateDefaultSubobject<UEnvironmentAttribute>(TEXT("Environment Attribute"));

}

void AEnvironmentActor::BeginPlay()
{
	Super::BeginPlay();
	World = GetWorld();
	SetActorLocation(LineTrace());
	EnvironmentMesh->SetCollisionObjectType(ECC_GameTraceChannel2);
}

void AEnvironmentActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnvironmentActor::DisableCollision()
{
	Capsule->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnvironmentActor::EnablePawnCollision()
{
	Capsule->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
	Capsule->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void AEnvironmentActor::StartRenewal()
{
	if (ActorAttributes) {
		if (CheckIfDestroyableActor()) {
			Destroy();
		}
		else {
			ActorAttributes->ActorStatus = EEnvStatus::EES_RegrowingResource;
			GetWorldTimerManager().SetTimer(RenewalTimer, this, &AEnvironmentActor::ClearRenewalTimer, ActorAttributes->RenewalTime);
		}
	}
	
}

bool AEnvironmentActor::CheckIfDestroyableActor()
{
	return ActorAttributes->ActorType == EEnvActorType::EEAT_Grass || ActorAttributes->ActorType == EEnvActorType::EEAT_Fruit;
}

void AEnvironmentActor::ClearRenewalTimer()
{
	if (ActorAttributes) {
		ActorAttributes->ActorStatus = EEnvStatus::EES_HasResource;
		GetWorldTimerManager().ClearTimer(RenewalTimer);
	}
}


FVector AEnvironmentActor::LineTrace()
{
	TraceStart = GetRootComponent()->GetComponentLocation();
	TraceEnd = FVector(TraceStart.X, TraceStart.Y, 0.f);
	if (World) {
		bool LineTrace = World->LineTraceSingleByChannel(
			HitResult,
			TraceStart,
			TraceEnd,
			ECollisionChannel::ECC_Visibility
		);
		if (LineTrace) {
			ImpactPoint = HitResult.ImpactPoint;
			NormalPoint = HitResult.ImpactNormal;
		}
	}
	return ImpactPoint;
}


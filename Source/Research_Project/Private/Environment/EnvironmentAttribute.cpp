// Fill out your copyright notice in the Description page of Project Settings.


#include "Environment/EnvironmentAttribute.h"

UEnvironmentAttribute::UEnvironmentAttribute()
{
	PrimaryComponentTick.bCanEverTick = true;

	ActorStatus = EEnvStatus::EES_HasResource;
}


void UEnvironmentAttribute::BeginPlay()
{
	Super::BeginPlay();
}

void UEnvironmentAttribute::Renew()
{
	if (ActorStatus == EEnvStatus::EES_RegrowingResource) ActorStatus = EEnvStatus::EES_HasResource;
}


void UEnvironmentAttribute::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}


// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnvStatus.h"
#include "EnvironmentAttribute.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RESEARCH_PROJECT_API UEnvironmentAttribute : public UActorComponent
{
	GENERATED_BODY()

public:	
	UEnvironmentAttribute();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//getters and setters
	FORCEINLINE EEnvActorType GetType() { return ActorType; }
	FORCEINLINE EEnvStatus GetStatus() { return ActorStatus; }
	FORCEINLINE void SetStatus(EEnvStatus Status) { ActorStatus = Status; }

protected:
	virtual void BeginPlay() override;

public:

	UPROPERTY(EditAnywhere, Category = "Environment Actor Properties")
	float RenewalTime;

	//Actor attributes

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Environment Actor Properties")
	EEnvActorType ActorType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Environment Actor Properties")
	EEnvStatus ActorStatus;

	void Renew();
};

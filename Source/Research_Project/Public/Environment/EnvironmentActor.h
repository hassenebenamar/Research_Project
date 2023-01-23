// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnvStatus.h"
#include "EnvironmentActor.generated.h"

class UEnvironmentAttribute;
UCLASS()
class RESEARCH_PROJECT_API AEnvironmentActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AEnvironmentActor();
	UWorld* World;

	//timer

	FTimerHandle RenewalTimer;

	void StartRenewal();
	bool CheckIfDestroyableActor();
	void ClearRenewalTimer();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	/*Setup collisions*/
	UFUNCTION(BlueprintCallable)
	virtual void DisableCollision();
	UFUNCTION(BlueprintCallable)
	virtual void EnablePawnCollision();

	/*Variables for other classes*/

	//environment attributes
	UPROPERTY(EditAnywhere)
	UEnvironmentAttribute* ActorAttributes;

private:

	/*3D representation for the placable actor*/
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* EnvironmentMesh;
	UPROPERTY(EditDefaultsOnly)
	class UCapsuleComponent* Capsule;

	// line trace 

	FHitResult HitResult;
	FVector TraceStart;
	FVector TraceEnd;

	TArray<AActor*> ActorsToIgnore;

	UPROPERTY(VisibleAnywhere)
	FVector ImpactPoint;
	UPROPERTY(VisibleAnywhere)
	FVector NormalPoint;

	//function
	FVector LineTrace();


};

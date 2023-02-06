// Fill out your copyright notice in the Description page of Project Settings.


#include "Agents/AgentSpawner.h"
#include "Agents/Agent.h"
#include "Agents/AgentAttribute.h"
#include "Research_Project/DebugMacros.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"

AAgentSpawner::AAgentSpawner()
{
	PrimaryActorTick.bCanEverTick = true;
	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Agent Spawn Box"));
	SetRootComponent(Box);
}

void AAgentSpawner::BeginPlay()
{
	Super::BeginPlay();
	World = GetWorld();
	if (Box) {
		Box->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	}
	SpawnActorsStart();
	GetAllAgents();
	StartGenerationTimer();
}

void AAgentSpawner::SpawnActorsStart()
{
	if (TotalToSpawn > 0) {
		for (uint8 i = 1; i <= TotalToSpawn; i++) {
			PRINT_STRING(TEXT("Spawning agents start"))
			FVector Location = FVector(CalculatePerlinX(), CalculatePerlinY(), 50.f);
			SpawnActorAtLocation(Location);
		}
	}
}


void AAgentSpawner::SpawnActorAtLocation(FVector Location)
{
	if (World && AgentsToSpawn.Num() > 0) {
		const uint8 selection = FMath::RandRange(0, AgentsToSpawn.Num() - 1);
		World->SpawnActor<AAgent>(AgentsToSpawn[selection], Location, FRotator(0.f, 0.f, 0.f));
	}
}

void AAgentSpawner::SpawnActorGenerated(AAgent* ActorToSpawn, FVector Location)
{
	if (World) {
		FActorSpawnParameters SpawnParameters;
		UClass* MyActorBPClass = StaticLoadClass(ActorToSpawn->StaticClass(), NULL, TEXT("BP_Agent"), NULL, LOAD_None, NULL);
		World->SpawnActor<AAgent>(MyActorBPClass, Location, FRotator(0.f, 0.f, 0.f), SpawnParameters);
	}
}

float AAgentSpawner::CalculatePerlinX()
{
	float RandomDisplacementX = FMath::RandRange((PerlinSeed * MinPerlin) - MaxSpawnRange, (PerlinSeed * MaxPerlin) + MaxSpawnRange);
	const float DisplacementX = FMath::Clamp(RandomDisplacementX, -MaxSpawnRange, MaxSpawnRange);
	return DisplacementX;
}

float AAgentSpawner::CalculatePerlinY()
{
	float RandomDisplacementY = FMath::RandRange((PerlinSeed * MinPerlin) - MaxSpawnRange, (PerlinSeed * MaxPerlin) + MaxSpawnRange);
	const float DisplacementY = FMath::Clamp(RandomDisplacementY, -MaxSpawnRange, MaxSpawnRange);
	return DisplacementY;
}

void AAgentSpawner::Selection()
{
	if (Population.Num() == 0) GetAllAgents();
	if (CheckPopulation()) return;;
	for (uint8 i = 0; i < Population.Num(); i++) {
		uint8 SelectionFirst = UKismetMathLibrary::RandomInteger(Population.Num() - 1);
		uint8 SelectionSecond = UKismetMathLibrary::RandomInteger(Population.Num() - 1);
		FirstCandidate = Population[SelectionFirst];
		SecondCandidate = Population[SelectionSecond];
		PRINT_STRING(UKismetSystemLibrary::GetObjectName(FirstCandidate))
		PRINT_STRING(UKismetSystemLibrary::GetObjectName(SecondCandidate))
		if (FirstCandidate->Attributes && SecondCandidate->Attributes) {
			UAgentAttribute* FirstAttribute = FirstCandidate->Attributes;
			UAgentAttribute* SecondAttribute = SecondCandidate->Attributes;
			if (FirstAttribute->SurvivabilityScore > SecondAttribute->SurvivabilityScore) {
				Population.Remove(FirstCandidate);
				Population.Remove(SecondCandidate);
				NewPopulation.Add(FirstCandidate);
			}
			else {
				Population.Remove(FirstCandidate);
				Population.Remove(SecondCandidate);
				NewPopulation.Add(SecondCandidate);
			}
		}
	}
	Crossover();
}

void AAgentSpawner::Crossover()
{
	//Use uniform crossover (every chromosome has a chance to be in the child 1 or 2)
	//Add a specie check or just duplicate the spawners. Check impact on program performance.
	PRINT_STRING(TEXT("Starting crossover"))
	if (CheckNewPopulation()) return;;
	for (uint8 i = 0; i < NewPopulation.Num() - 1; i++) {
		uint8 SelectionFirst = UKismetMathLibrary::RandomInteger(NewPopulation.Num() - 1);
		uint8 SelectionSecond = UKismetMathLibrary::RandomInteger(NewPopulation.Num() - 1);
		AAgent* FirstParent = Population[SelectionFirst];
		AAgent* SecondParent = Population[SelectionSecond];
		ChildOne = FirstParent;
		ChildTwo = NewObject<AAgent>(this, "ChildTwo");
		if (FirstParent->Attributes && SecondParent->Attributes) {
			UAgentAttribute* FirstAttribute = FirstParent->Attributes;
			UAgentAttribute* SecondAttribute = SecondParent->Attributes;
			for (uint8 j = 0; j <= FirstAttribute->DNA.Num() - 1; j++) {
				CrossoverChance = UKismetMathLibrary::RandomFloat();
				
				if (CrossoverChance >= 0.5f) {
					if (ChildOne->Attributes && ChildTwo->Attributes) {
						ChildOne->Attributes->DNA[j] = FirstAttribute->DNA[j];
						ChildTwo->Attributes->DNA[j] = SecondAttribute->DNA[j];
					}
				}
				else {
					if (ChildOne->Attributes && ChildTwo->Attributes) {
						ChildTwo->Attributes->DNA[j] = FirstAttribute->DNA[j];
						ChildOne->Attributes->DNA[j] = SecondAttribute->DNA[j];
					}
				}
			}
			RemoveParentsFromPopulation(FirstParent, SecondParent);
			DestroyParents(FirstParent, SecondParent);
			GetAllAgents();
			MutateChildren();
			SpawnChildren();
			
		}
	}
	ClearGenerationTimer();
	StartGenerationTimer();
}

bool AAgentSpawner::CheckNewPopulation()
{
	return NewPopulation.Num() == 0 && NewPopulation.Num() % 2 > 0;
}

bool AAgentSpawner::CheckPopulation()
{
	return Population.Num() == 0 && Population.Num() % 2 > 0;
}

void AAgentSpawner::MutateChildren()
{
	if (ChildOne == nullptr || ChildTwo == nullptr) return;
	Mutation(ChildOne);
	Mutation(ChildTwo);
}

void AAgentSpawner::DestroyParents(AAgent*& FirstParent, AAgent*& SecondParent)
{
	FirstParent->Destroy();
	SecondParent->Destroy();
	PRINT_STRING(UKismetSystemLibrary::GetObjectName(FirstParent))
	PRINT_STRING(UKismetSystemLibrary::GetObjectName(SecondParent))
}

void AAgentSpawner::RemoveParentsFromPopulation(AAgent*& FirstParent, AAgent*& SecondParent)
{
	Population.Remove(FirstParent);
	Population.Remove(SecondParent);
}

void AAgentSpawner::SpawnChildren()
{
	FVector LocationOne = FVector(CalculatePerlinX(), CalculatePerlinY(), 50.f);
	FVector LocationTwo = FVector(CalculatePerlinX(), CalculatePerlinY(), 50.f);
	SpawnActorGenerated(ChildOne, LocationOne);
	SpawnActorGenerated(ChildTwo, LocationTwo);
}

void AAgentSpawner::Mutation(AAgent* AgentToMutate)
{
	if (AgentToMutate->Attributes == nullptr) return;
	for (uint8 i = 0; i < AgentToMutate->Attributes->DNA.Num() - 1; i++) {
		MutationChance = UKismetMathLibrary::RandomFloat();
		if (MutationChance <= 0.10f) {
			if (AgentToMutate->Attributes->DNA[i] == 0) AgentToMutate->Attributes->DNA[i]++;
			else AgentToMutate->Attributes->DNA[i]--;
		}
	}
}

TArray<AAgent*> AAgentSpawner::GetAllAgents()
{
	//credit for the overlapobjecttypes https://gist.github.com/JoshLmao/723bde5dd4b8c650c116e0d2ada66cce

	if (World == nullptr) return Population;

	TArray<TEnumAsByte<EObjectTypeQuery>> OverlapObjectTypes;
	OverlapObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
	
	UKismetSystemLibrary::BoxOverlapActors(World, GetActorLocation(), Box->GetScaledBoxExtent(), OverlapObjectTypes, AAgent::StaticClass(), ActorsToIgnore, OutActors);
	for (int32 NumOfAgents = 0; NumOfAgents < OutActors.Num(); NumOfAgents++) {
		AAgent* FoundAgent = Cast<AAgent>(OutActors[NumOfAgents]);
		Population.AddUnique(FoundAgent);
	}
	return Population;
}

void AAgentSpawner::StartGenerationTimer()
{
	PRINT_STRING(TEXT("Starting Timer"))
	GetWorldTimerManager().SetTimer(GenerationTimer, this, &AAgentSpawner::Selection, GenerationTime);
}

void AAgentSpawner::ClearGenerationTimer()
{
	GetWorldTimerManager().ClearTimer(GenerationTimer);
}

void AAgentSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAgentSpawner::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


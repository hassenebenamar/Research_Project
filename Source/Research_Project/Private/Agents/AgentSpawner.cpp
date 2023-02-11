// Fill out your copyright notice in the Description page of Project Settings.


#include "Agents/AgentSpawner.h"
#include "Agents/AgentsHeader.h"
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
	PRINT_STRING(TEXT("Spawning agents start"))
	if (TotalToSpawn > 0) {
		for (uint8 i = 1; i <= TotalToSpawn; i++) {
			
			FVector Location = FVector(CalculatePerlinX(), CalculatePerlinY(), 50.f);
			World->SpawnActor<AAgent>(ClassToSpawn, Location, FRotator(0.f, 0.f, 0.f));
			
		}
	}
	PRINT_STRING(TEXT("Spawning Agents end"))
}

void AAgentSpawner::SpawnActorGenerated(AAgent* ActorToSpawn, FVector Location)
{
	if (World && ActorToSpawn) {
		FActorSpawnParameters SpawnParameters;
		if (ClassToSpawn == ActorToSpawn->GetClass()) {
			UClass* AgentClass = ActorToSpawn->StaticClass();
			World->SpawnActor<AAgent>(AgentClass, Location, FRotator(0.f, 0.f, 0.f), SpawnParameters);
		}
		
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
	PRINT_STRING(TEXT("Selection process is going to start"))
	if (Population.Num() == 0) GetAllAgents();
	TArray<AAgent*> CopyPop;
	int32 i = 0;
	PRINT_INT(i)
	for (int32 j = 0; j <= Population.Num() - 1; j++) {
		CopyPop.Add(Population[j]);
	}
	for (i; i <= CopyPop.Num() - 1; i++) {
		PRINT_STRING(TEXT("Starting selection process"))
		uint8 SelectionFirst = UKismetMathLibrary::RandomInteger(Population.Num() - 1);
		uint8 SelectionSecond;
		do {
			SelectionSecond = UKismetMathLibrary::RandomInteger(Population.Num() - 1);
		} while (SelectionSecond == SelectionFirst);
		PRINT_INT(SelectionFirst)
		PRINT_INT(SelectionSecond)
		FirstCandidate = Population[SelectionFirst];
		SecondCandidate = Population[SelectionSecond];
		FString DebugText = UKismetSystemLibrary::GetObjectName(FirstCandidate);
		FString DebugTextTwo = UKismetSystemLibrary::GetObjectName(SecondCandidate);
		PRINT_STRING(DebugText)
		PRINT_STRING(DebugTextTwo)
		
		if (FirstCandidate != nullptr && SecondCandidate == nullptr) {
			if (FirstCandidate->GetClass() == ClassToSpawn) {
				Population.RemoveAt(SelectionFirst);
				NewPopulation.Add(FirstCandidate);
			}
		}
		if (FirstCandidate == nullptr && SecondCandidate != nullptr) {
			if (SecondCandidate->GetClass() == ClassToSpawn) {
				Population.RemoveAt(SelectionSecond);
				NewPopulation.Add(SecondCandidate);
			}
		}
		if (FirstCandidate != nullptr && SecondCandidate != nullptr) {
			PRINT_STRING(TEXT("Hi, 1"))
			if (FirstCandidate->GetClass() == ClassToSpawn && SecondCandidate->GetClass() == ClassToSpawn) {
				PRINT_STRING(TEXT("Hi, 2"))
				if (FirstCandidate->Attributes && SecondCandidate->Attributes) {
					PRINT_STRING(TEXT("Hi, 3"))  
					UAgentAttribute* FirstAttribute = FirstCandidate->Attributes;
					UAgentAttribute* SecondAttribute = SecondCandidate->Attributes;
					if (FirstAttribute->SurvivabilityScore > SecondAttribute->SurvivabilityScore) {
						PRINT_STRING(TEXT("Removing first cand from pop"))
						Population.RemoveAt(SelectionFirst);
						NewPopulation.Add(FirstCandidate);
					}
					else {
						PRINT_STRING(TEXT("Removing sec cand from pop"))
						Population.RemoveAt(SelectionSecond);
						NewPopulation.Add(SecondCandidate);
					}
					
				}
			}
		}
		PRINT_INT(Population.Num() - 1)
		PRINT_INT(i)
	}
	PRINT_STRING("Selection done")
	return;
	//Crossover();
}

void AAgentSpawner::Crossover()
{
	//Use uniform crossover (every chromosome has a chance to be in the child 1 or 2)
	//Add a specie check or just duplicate the spawners. Check impact on program performance.
	PRINT_STRING(TEXT("Starting crossover"))
	if (CheckNewPopulation()) return;;
	for (int32 i = 0; i < NewPopulation.Num(); i++) {
		AAgent* FirstParent = Population[i];
		AAgent* SecondParent = Population[i+1];
		ChildOne = NewObject<AAgent>(GetWorld(), ClassToSpawn, FName("ChildOne" + i));
		ChildTwo = NewObject<AAgent>(GetWorld(), ClassToSpawn, FName("ChildTwo" + i));
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
			//MutateChildren();
			//SpawnChildren();
			
		}
	}
	ClearGenerationTimer();
	StartGenerationTimer();
}

bool AAgentSpawner::CheckNewPopulation()
{
	return NewPopulation.Num() == 0;
}

void AAgentSpawner::MutateChildren()
{
	if (ChildOne == nullptr || ChildTwo == nullptr) return;
	Mutation(ChildOne);
	Mutation(ChildTwo);
}

void AAgentSpawner::DestroyParents(AAgent* FirstParent, AAgent* SecondParent)
{
	FirstParent->Destroy();
	SecondParent->Destroy();
	PRINT_STRING(UKismetSystemLibrary::GetObjectName(FirstParent))
	PRINT_STRING(UKismetSystemLibrary::GetObjectName(SecondParent))
}

void AAgentSpawner::RemoveParentsFromPopulation(AAgent* FirstParent, AAgent* SecondParent)
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
		if (FoundAgent)
		{
			TSubclassOf<AAgent> FoundAgentClass = FoundAgent->GetClass();
			if (FoundAgentClass == ClassToSpawn) {
				Population.Add(FoundAgent);
				
			}
		}
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


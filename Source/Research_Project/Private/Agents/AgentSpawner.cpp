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

void AAgentSpawner::SpawnOffspring(TArray<AAgent*> ActorsToSpawn)
{
	if (World && ActorsToSpawn.Num() > 0) {
		FActorSpawnParameters SpawnParameters;
		for (int32 i = 0; i < ActorsToSpawn.Num(); i++) {
			AAgent* ActorToSpawn = ActorsToSpawn[i];
			if (ActorToSpawn != nullptr) {
				UClass* AgentClass = ActorToSpawn->GetClass();
				if (ClassToSpawn == AgentClass) {
					AAgent* NewAgent;
					do {
						FVector Location = FVector(CalculatePerlinX(), CalculatePerlinY(), 50.f);
						NewAgent = World->SpawnActor<AAgent>(AgentClass, Location, FRotator(0.f, 0.f, 0.f), SpawnParameters);
					} while (NewAgent == nullptr);
					NewAgent->CopyObject(*ActorToSpawn);
					NewAgent->Tags.Add(TEXT("Spawned Agent"));
					ActorsSpawned.Add(NewAgent);
				}
			}
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
	for (int32 j = 0; j <= Population.Num() - 1; j++) {
		CopyPop.Add(Population[j]);
	}
	for (i; i < CopyPop.Num(); i++) {
		uint8 SelectionFirst = UKismetMathLibrary::RandomInteger(Population.Num());
		uint8 SelectionSecond;
		do {
			if (Population.Num() == 1) {
				SelectionFirst = 0;
				SelectionSecond = 2;
				FirstCandidate = Population[SelectionFirst];
				SecondCandidate = nullptr;
			}
			else {
				SelectionSecond = UKismetMathLibrary::RandomInteger(Population.Num());
				FirstCandidate = Population[SelectionFirst];
				SecondCandidate = Population[SelectionSecond];
			}
		} while (SelectionSecond == SelectionFirst);
		//FString DebugText = UKismetSystemLibrary::GetObjectName(FirstCandidate);
		//FString DebugTextTwo = UKismetSystemLibrary::GetObjectName(SecondCandidate);
		
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
			if (FirstCandidate->GetClass() == ClassToSpawn && SecondCandidate->GetClass() == ClassToSpawn) {
				if (FirstCandidate->Attributes && SecondCandidate->Attributes) {
					UAgentAttribute* FirstAttribute = FirstCandidate->Attributes;
					UAgentAttribute* SecondAttribute = SecondCandidate->Attributes;
					if (FirstAttribute->SurvivabilityScore > SecondAttribute->SurvivabilityScore) {
						Population.RemoveAt(SelectionFirst);
						NewPopulation.Add(FirstCandidate);
					}
					else {
						Population.RemoveAt(SelectionSecond);
						NewPopulation.Add(SecondCandidate);
					}
					
				}
			}
		}
	}
	PRINT_STRING("Selection done")
	RemoveLastParentIfNeeded();
	Crossover();
}

void AAgentSpawner::Crossover()
{
	//Use uniform crossover (every chromosome has a chance to be in the child 1 or 2)
	//Add a specie check or just duplicate the spawners. Check impact on program performance.
	PRINT_STRING(TEXT("Starting crossover"))
	if (CheckNewPopulation()) return;
	for (int32 j = 0; j <= NewPopulation.Num() - 1; j++) {
		Population.Add(NewPopulation[j]);
	}
	PRINT_INT(Population.Num())
	int32 i = 0;
	int32 PopNum = Population.Num() - 1;
	while (NewPopulation.Num() > 1) {
		AAgent* FirstParent = NewPopulation[0];
		AAgent* SecondParent = NewPopulation[1];
		ChildOne = NewObject<AAgent>(GetWorld(), ClassToSpawn);
		ChildOne->Tags.Add(TEXT("Offspring"));
		ChildTwo = NewObject<AAgent>(GetWorld(), ClassToSpawn);
		ChildTwo->Tags.Add(TEXT("Offspring"));
		if (FirstParent->Attributes && SecondParent->Attributes) {
			UAgentAttribute* FirstAttribute = FirstParent->Attributes;
			UAgentAttribute* SecondAttribute = SecondParent->Attributes;
			for (uint8 j = 0; j <= FirstAttribute->DNA.Num() - 1; j++) {
				CrossoverChance = UKismetMathLibrary::RandomFloat();
				if (CrossoverChance >= 0.5f) {
					if (ChildOne->Attributes && ChildTwo->Attributes) {
						FString DebugText = TEXT("ChildOne dna crossover");
						ChildOne->Attributes->DNA.Add(FirstAttribute->DNA[j]);
						ChildTwo->Attributes->DNA.Add(SecondAttribute->DNA[j]);
					}
				}
				else {
					if (ChildOne->Attributes && ChildTwo->Attributes) {
						FString DebugText = TEXT("ChildTwo dna crossover");
						ChildTwo->Attributes->DNA.Add(FirstAttribute->DNA[j]);
						ChildOne->Attributes->DNA.Add(SecondAttribute->DNA[j]);
					}
				}
			}
			NewPopulation.Remove(FirstParent);
			NewPopulation.Remove(SecondParent);
			FirstParent->Destroy(true);
			SecondParent->Destroy(true);
			/*PRINT_STRING(UKismetSystemLibrary::GetObjectName(FirstParent))
			PRINT_STRING(UKismetSystemLibrary::GetObjectName(SecondParent))*/
			MutateChildren();
		}
	}
	PRINT_STRING(TEXT("Crossover done"))
	PRINT_STRING(TEXT("Time to spawn the new population offspring"))
	SpawnOffspring(OffspringPopulation);
	return;
	ClearGenerationTimer();
	StartGenerationTimer();
}

bool AAgentSpawner::CheckNewPopulation()
{
	return NewPopulation.Num() == 0;
}

void AAgentSpawner::RemoveLastParentIfNeeded()
{
	//this function is to trim the new population in case there is an agent that's gonna end up alone in the crossover process
	if (NewPopulation.Num() % 2 > 0) {
		AAgent* AgentToRemove = NewPopulation[NewPopulation.Num() - 1];
		if (AgentToRemove) {
			NewPopulation.Remove(AgentToRemove);
			AgentToRemove->Destroy();
		}
	}
}

void AAgentSpawner::MutateChildren()
{
	if (ChildOne == nullptr || ChildTwo == nullptr) return;

	//Mutation process and adding children to the offspring Tarray for spawning functions
	Mutation(ChildOne);
	OffspringPopulation.Add(ChildOne);
	Mutation(ChildTwo);
	OffspringPopulation.Add(ChildTwo);
}

void AAgentSpawner::Mutation(AAgent* AgentToMutate)
{
	if (AgentToMutate->Attributes == nullptr) return;
	for (uint8 i = 0; i < AgentToMutate->Attributes->DNA.Num(); i++) {
		MutationChance = UKismetMathLibrary::RandomFloat();
		if (MutationChance > 0.79f) {
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


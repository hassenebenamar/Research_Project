// Fill out your copyright notice in the Description page of Project Settings.


#include "Agents/AgentSpawner.h"
#include "Agents/AgentsHeader.h"
#include "Controllers/AgentController.h"
#include "Research_Project/DebugMacros.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
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
	StartGenerationTimer();
}

void AAgentSpawner::SpawnActorsStart()
{
	PRINT_STRING(TEXT("Spawning agents start"))
	if (TotalToSpawn > 0) {
		for (uint8 i = 1; i <= TotalToSpawn; i++) {
			AAgent* NewAgent;
			do {
				FVector Location = FVector(CalculatePerlinX(), CalculatePerlinY(), 50.f);
				NewAgent = World->SpawnActor<AAgent>(ClassToSpawn, Location, FRotator(0.f, 0.f, 0.f));
			} while (NewAgent == nullptr);
			if (NewAgent && NewAgent->Attributes) NewAgent->Attributes->InitializeNewStats(NewAgent);
			
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
						FTransform ActorTransform = FTransform(FRotator(0.f, 0.f, 0.f), Location, ActorToSpawn->GetActorScale3D());
						NewAgent = World->SpawnActor<AAgent>(AgentClass, ActorTransform, SpawnParameters);
					} while (NewAgent == nullptr);
					if (NewAgent) {
						NewAgent->CopyObject(*ActorToSpawn);
						if (NewAgent->Attributes) NewAgent->Attributes->SetAgentState(EAnimalState::EAS_Active);
						ActorsSpawned.Add(NewAgent);
					}
					
				}
			}
		}
		OffspringPopulation.Empty();
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
	if (TerminationCriterion == 50) return;
	GetAllAgents();
	TArray<AAgent*> CopyPop;
	int32 i = 0;
	for (int32 j = 0; j <= Population.Num() - 1; j++) {
		CopyPop.Add(Population[j]);
	}
	FString DebugString = TEXT("Generation: ") + FString::FromInt(TerminationCriterion);
	PRINT_STRING_BLUE(DebugString)
	PRINT_STRING_BLUE(TEXT("Savings Agents' Attributes"))
	//SaveAgentsAttributes();
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
		ChildOne = NewObject<AAgent>(this, ClassToSpawn);
		ChildOne->Tags.Add(TEXT("Offspring"));
		ChildTwo = NewObject<AAgent>(this, ClassToSpawn);
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
			if (ChildOne->Attributes && ChildTwo->Attributes) {
				ChildOne->Attributes->InitializeNewStats(ChildOne);
				ChildTwo->Attributes->InitializeNewStats(ChildTwo);
			}
		}
	}
	PRINT_STRING(TEXT("Crossover done"))
	PRINT_STRING(TEXT("Time to spawn the new population offspring"))
	SpawnOffspring(OffspringPopulation);
	TerminationCriterion++;
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
		if (MutationChance > 0.83f) {
			if (AgentToMutate->Attributes->DNA[i] == 0) AgentToMutate->Attributes->DNA[i]++;
			else AgentToMutate->Attributes->DNA[i]--;
		}
	}
}

TArray<AAgent*> AAgentSpawner::GetAllAgents()
{
	//credit for the overlapobjecttypes https://gist.github.com/JoshLmao/723bde5dd4b8c650c116e0d2ada66cce

	if (World == nullptr) return Population;
	
	Population.Empty();

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

	SaveAgentsAttributes();

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

void AAgentSpawner::SaveAgentsAttributes()
{
	FString ClassToPrint = ClassToSpawn->GetName();
	FString SaveDirectory = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir()) + TEXT("AgentData/");
	FString FileName = FString::FromInt(FDateTime::Now().ToUnixTimestamp()) + ClassToPrint + TEXT("_AgentData.txt");
	FString SavePath = SaveDirectory + FileName;

	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	if (!PlatformFile.DirectoryExists(*SaveDirectory))
	{
		PlatformFile.CreateDirectory(*SaveDirectory);
	}

	FString SaveString = "";
	if (!Population.Num())
	{
		UE_LOG(LogTemp, Warning, TEXT("Population is empty!"));
		return;
	}
	for (uint8 index = 0; index < Population.Num(); index++)
	{
		AAgent* Agent = Population[index];
		if (!Agent || !Agent->Attributes)
		{
			UE_LOG(LogTemp, Warning, TEXT("Invalid agent or missing attributes!"));
			continue;
		}
		UAgentAttribute* Attribute = Agent->Attributes;
		const UEnum* SpecieEnum = FindObject<UEnum>(ANY_PACKAGE, TEXT("EAgentSpecie"), true);
		FString SpecieString = SpecieEnum ? SpecieEnum->GetNameStringByValue(static_cast<int64>(Attribute->AgentSpecie)) : TEXT("Unknown");
		FString AgentData = FString::Printf(TEXT("Agent ID: %d, Index: %d, Specie: %s, Survivability Score: %f, Generation: %d, DNA: ["), Attribute->AgentID, index, *SpecieString, Attribute->SurvivabilityScore, TerminationCriterion);
		for (uint8 Index = 0; Index < Attribute->DNA.Num(); Index++)
		{
			uint8 DNAGene = Attribute->DNA[Index];
			AgentData += FString::Printf(TEXT("%d"), DNAGene);
			if (Index < Attribute->DNA.Num() - 1)
			{
				AgentData += TEXT(", ");
			}
		}
		AgentData += TEXT("]\n");
		SaveString += AgentData;
	}

	FFileHelper::SaveStringToFile(SaveString, *SavePath);

	PRINT_STRING("Agents' attributes saved to file")
}

void AAgentSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAgentSpawner::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


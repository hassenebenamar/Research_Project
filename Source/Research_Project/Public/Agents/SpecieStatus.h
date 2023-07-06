#pragma once

UENUM(BlueprintType)
enum class EAnimalState : uint8 {

	EAS_Alive UMETA(DisplayName = "Alive"),
	EAS_Sleeping UMETA(DisplayName = "Sleeping"),
	EAS_Active UMETA(DisplayName = "Active"),
	EAS_Eating UMETA(DisplayName = "Eating"),
	EAS_Hunting UMETA(DisplayName = "Hunting"),
	EAS_Chasing UMETA(DisplayName = "Chasing"),
	EAS_Attacking UMETA(DisplayName = "Attacking"),
	EAS_Dead UMETA(DisplayName = "Dead")
};

UENUM(BlueprintType)
enum class EAgentSpecie : uint8 {
	EAS_Deer UMETA(DisplayName = "Deer"),
	EAS_Fox UMETA(DisplayName = "Fox")
};
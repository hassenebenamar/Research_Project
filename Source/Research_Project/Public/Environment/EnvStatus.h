#pragma once

UENUM(BlueprintType)
enum class EEnvStatus : uint8 {

	EES_RegrowingResource UMETA(DisplayName = "Regrowing Resource"),
	EES_HasResource UMETA(DisplayName = "Has Resource")

};

UENUM(BlueprintType)
enum class EEnvType : uint8 {

	EET_Forest UMETA(DisplayName = "Forest"),
	EET_Plain UMETA(DisplayName = "Plain"),
	EET_Plateau UMETA(DisplayName = "Plateau"), 
	EET_MountainRange UMETA(DisplayName = "Mountain Range"),
	EET_River UMETA(DisplayName = "River"),
	EET_Ocean UMETA(DisplayName = "Ocean")
	
};

UENUM(BlueprintType)
enum class EEnvActorType : uint8 {
	EEAT_Grass UMETA(DisplayName = "Grass"),
	EEAT_Fruit UMETA(DisplayName = "Fruit"),
	EEAT_FruitTree UMETA(DisplayName = "Fruit Tree")
};


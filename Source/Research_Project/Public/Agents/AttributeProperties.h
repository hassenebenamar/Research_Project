#pragma once

UENUM(BlueprintType)
enum class EFloatPropertyName : uint8 {
	EFPN_SightRange UMETA(DisplayName = "Sight Range"),
	EFPN_SightOffset UMETA(DisplayName = "Sight Offset"),
	EFPN_SightRadius UMETA(DisplayName = "Sight Radius"),
	EFPN_HearingThreshold UMETA(DisplayName = "Hearing Threshold")
};
#pragma once
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetStringLibrary.h"

#define PRINT_STRING(String) { \
	if (this) { \
		UKismetSystemLibrary::PrintString(this, String, true, true, FColor::Red, 30.f); \
	}\
 }
#define PRINT_FLOAT(FloatToPrint) { \
	if (this) { \
		FString DebugString = FString::SanitizeFloat(FloatToPrint); \
		UKismetSystemLibrary::PrintString(this, DebugString, true, true, FColor::Blue, 30.f); \
	} \
}
#define PRINT_INT(IntToPrint) { \
	if (this) { \
		FString DebugString = FString::FromInt(IntToPrint); \
		UKismetSystemLibrary::PrintString(this, DebugString, true, true, FColor::Purple, 30.f); \
	} \
}

#define PRINT_BOOL(BoolToPrint) { \
	if (this) { \
		uint8 IntToPrint = UKismetMathLibrary::Conv_BoolToInt(BoolToPrint); \
		FString DebugString = FString::FromInt(IntToPrint); \
		UKismetSystemLibrary::PrintString(this, DebugString, true, true, FColor::Purple, 30.f); \
	} \
}
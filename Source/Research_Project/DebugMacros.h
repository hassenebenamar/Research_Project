#pragma once
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetStringLibrary.h"

#define PRINT_STRING(String) { \
	if (this) { \
		UKismetSystemLibrary::PrintString(this, String, true, true, FColor::Red, 30.f); \
	}\
 }

#define PRINT_STRING_BLUE(String) { \
	if (this) { \
		UKismetSystemLibrary::PrintString(this, String, true, true, FColor::Blue, 30.f); \
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
		UKismetSystemLibrary::PrintString(this, DebugString, true, true, FColor::Green, 30.f); \
	} \
}

#define PRINT_BOOL(BoolToPrint) { \
	if (this) { \
		if (BoolToPrint) { \
			FString DebugString = "true"; \
			UKismetSystemLibrary::PrintString(this, DebugString, true, true, FColor::Purple, 30.f); \
		} else { \
			FString DebugString = "false"; \
			UKismetSystemLibrary::PrintString(this, DebugString, true, true, FColor::Purple, 30.f); \
		} \
	}\
}
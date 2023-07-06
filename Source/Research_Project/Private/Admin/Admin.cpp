// Fill out your copyright notice in the Description page of Project Settings.


#include "Admin/Admin.h"

AAdmin::AAdmin()
{
	SetTickableWhenPaused(true);
}


void AAdmin::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}


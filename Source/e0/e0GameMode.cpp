// Copyright Epic Games, Inc. All Rights Reserved.

#include "e0GameMode.h"
#include "e0HUD.h"
#include "e0Character.h"
#include "UObject/ConstructorHelpers.h"

Ae0GameMode::Ae0GameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = Ae0HUD::StaticClass();
}

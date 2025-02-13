// Copyright Epic Games, Inc. All Rights Reserved.

#include "demo_ik_ue4GameMode.h"
#include "demo_ik_ue4Character.h"
#include "UObject/ConstructorHelpers.h"

Ademo_ik_ue4GameMode::Ademo_ik_ue4GameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

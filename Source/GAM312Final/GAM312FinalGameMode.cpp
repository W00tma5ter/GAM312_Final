// Copyright Epic Games, Inc. All Rights Reserved.

#include "GAM312FinalGameMode.h"
#include "GAM312FinalCharacter.h"
#include "MyPlayerHUD.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"

AGAM312FinalGameMode::AGAM312FinalGameMode()
	: Super()
{
	PrimaryActorTick.bCanEverTick = true;

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AMyPlayerHUD::StaticClass();
}

void AGAM312FinalGameMode::BeginPlay()
{
	Super::BeginPlay();

	// Set game state to playing.
	SetCurrentState(EGamePlayState::EPlaying);

	// Reference to current player character.
	MyCharacter = Cast<AGAM312FinalCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));

	// Setup starting menu.
	ChangeMenuWidget(StartingWidgetClass);
}

void AGAM312FinalGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	GetWorld()->GetMapName();

	// End game if player runs out of health.
	if (MyCharacter)
	{
		if (FMath::IsNearlyZero(MyCharacter->GetHealth(), 0.001f))
		{
			// Wait 5 seconds then reload game.
			FTimerHandle TempDelay;
			GetWorldTimerManager().SetTimer(TempDelay, this, &AGAM312FinalGameMode::ReloadGame, 5.0f, false);
		}
	}
}

EGamePlayState AGAM312FinalGameMode::GetCurrentState() const
{
	return CurrentState;
}

void AGAM312FinalGameMode::SetCurrentState(EGamePlayState NewState)
{
	CurrentState = NewState;
	HandleNewState(CurrentState);
}

void AGAM312FinalGameMode::HandleNewState(EGamePlayState NewState)
{
	switch (NewState)
	{
	case EGamePlayState::EPlaying:
	{
		// do nothing
	}
	break;
	// Unknown/default state
	case EGamePlayState::EGameOver:
	{
		UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
	}
	break;
	// Unknown/default state
	default:
	case EGamePlayState::EUnknown:
	{
		// do nothing
	}
	break;
	}
}

void AGAM312FinalGameMode::ReloadGame()
{
	SetCurrentState(EGamePlayState::EGameOver);
}

// Function to change the current visible widget.
void AGAM312FinalGameMode::ChangeMenuWidget(TSubclassOf<UUserWidget> NewWidgetClass)
{
	if (CurrentWidget != nullptr)
	{
		CurrentWidget->RemoveFromViewport();
		CurrentWidget = nullptr;
	}
	if (NewWidgetClass != nullptr)
	{
		CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), NewWidgetClass);
		if (CurrentWidget != nullptr)
		{
			CurrentWidget->AddToViewport();
		}
	}
}

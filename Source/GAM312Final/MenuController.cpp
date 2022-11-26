// Copyright 2022


#include "MenuController.h"

void AMenuController::BeginPlay()
{
    Super::BeginPlay();
    SetInputMode(FInputModeGameAndUI()); //Configure input mode.
}
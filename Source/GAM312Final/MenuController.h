// Copyright 2022

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MenuController.generated.h"

/**
 * This controller is for menu navigation. simply sets input mode to Game & UI
 */
UCLASS()
class GAM312FINAL_API AMenuController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	
};

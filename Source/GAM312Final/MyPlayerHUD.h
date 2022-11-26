// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "GAM312FinalCharacter.h"
#include "MyPlayerHUD.generated.h"

/**
 * 
 */
UCLASS()
class GAM312FINAL_API AMyPlayerHUD : public AHUD
{
	GENERATED_BODY()

public:
	AMyPlayerHUD();
	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

	virtual void BeginPlay() override;

private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;

	UPROPERTY(EditAnywhere, Category = "Health")
		TSubclassOf<class UUserWidget> HUDWidgetClass;

	UPROPERTY(EditAnywhere, Category = "Health")
		class UUserWidget* CurrentWidget;

};

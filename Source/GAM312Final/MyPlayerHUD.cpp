// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerHUD.h"
#include "Engine/Canvas.h"
#include "Engine/Texture2D.h"
#include "TextureResource.h"
#include "CanvasItem.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"


AMyPlayerHUD::AMyPlayerHUD()
{
	// Set the crosshair texture
	static ConstructorHelpers::FObjectFinder<UTexture2D> CrosshairTexObj(TEXT("/Game/UI/Crosshair"));
	CrosshairTex = CrosshairTexObj.Object;

	// Set player HUD widget.
	static ConstructorHelpers::FClassFinder<UUserWidget> HealthBarObj(TEXT("/Game/UI/UserInterface"));
	HUDWidgetClass = HealthBarObj.Class;

}

void AMyPlayerHUD::DrawHUD()
{
	Super::DrawHUD();

	// Draw custom crosshair.

	// find center of the Canvas.
	const FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);

	// offset by half the texture's dimensions so that the center of the texture aligns with the center of the Canvas.
	const FVector2D CrosshairDrawPosition((Center.X),
		(Center.Y + 50.0f));

	// draw the crosshair.
	FCanvasTileItem TileItem(CrosshairDrawPosition, CrosshairTex->GetResource(), FLinearColor::White);
	TileItem.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(TileItem);
}

void AMyPlayerHUD::BeginPlay()
{
	Super::BeginPlay();

	if (HUDWidgetClass != nullptr)
	{
		// Generate HUD widget.
		CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), HUDWidgetClass);

		if (CurrentWidget)
		{
			//Add HUD to screen.
			CurrentWidget->AddToViewport();
		}
	}
}

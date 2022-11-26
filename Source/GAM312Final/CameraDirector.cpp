// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraDirector.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ACameraDirector::ACameraDirector()
{
 	// Set this actor to call Tick() every frame.(DISABLED)
	//PrimaryActorTick.bCanEverTick = true;

    // Set default variable values.
    SmoothBlendTime = 0.75f;
    CameraNum = 0;
}

// Called when the game starts or when spawned
void ACameraDirector::BeginPlay()
{
	Super::BeginPlay();

    // Initialize input bindings.
    BindToInput();
    // Find the actor that handles control for the local player.
    OurPlayerController = UGameplayStatics::GetPlayerController(this, 0);
    OriginalCamera = OurPlayerController->GetViewTarget();
	
}

// Called every frame.
void ACameraDirector::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Input Bindings.
void ACameraDirector::BindToInput()
{
    // Initialize InputComponent
    InputComponent = NewObject<UInputComponent>(this);
    InputComponent->RegisterComponent();
    if (InputComponent)
    {
        // Bind action
        InputComponent->BindAction
        (
            "CameraToggle", // Input identifier
            IE_Pressed, // Reaction Type
            this, // Reaction Object
            &ACameraDirector::CameraToggleCallback // Reaction Function
        );
        // Connect InputComponent to PlayerController
        EnableInput(GetWorld()->GetFirstPlayerController());
    }
}

// Camera Toggle Callback Function.
void ACameraDirector::CameraToggleCallback()
{
    if (OurPlayerController)
    {
        // Cycle camera number.
        if ((CameraNum >= 2) || (CameraNum < 0))
        {CameraNum = 0;}
        else
        {CameraNum++;}

        // Activate camera based on current CameraNum value.
        switch (CameraNum)
        {
        case 0:
            // Move to Player Camera
            OurPlayerController->SetViewTargetWithBlend(OriginalCamera, SmoothBlendTime);
            break;
        case 1:
            // Move to Camera 1
            if ((OurPlayerController->GetViewTarget() != CameraOne) && (CameraOne != nullptr))
            {
                OurPlayerController->SetViewTargetWithBlend(CameraOne, SmoothBlendTime);
            }
            break;
        case 2:
            // Move to Camera 2
            if ((OurPlayerController->GetViewTarget() != CameraTwo) && (CameraTwo != nullptr))
            {
                OurPlayerController->SetViewTargetWithBlend(CameraTwo, SmoothBlendTime);
            }
            break;
        default:
            break;
        }
        
    }
}


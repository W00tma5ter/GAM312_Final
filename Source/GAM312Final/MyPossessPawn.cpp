// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPossessPawn.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/InputSettings.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine.h"
#include "DrawDebugHelpers.h"

// Sets default values
AMyPossessPawn::AMyPossessPawn()
{    
    // Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    // Create a dummy root component we can attach things to.
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

    // Create camera and visible object components.
    OurCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("OurCamera"));
    OurVisibleComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("OurVisibleComponent"));

    // Attach our camera and visible object to root component. Offset camera.
    OurCamera->SetupAttachment(RootComponent);
    OurCamera->SetRelativeLocation(FVector(-250.0f, 1.75f, 150.0f));
    OurCamera->bUsePawnControlRotation = true;
    OurVisibleComponent->SetupAttachment(RootComponent);
    this->bUseControllerRotationPitch = false;
    this->bUseControllerRotationYaw = true;

    // Set turn rate for mouse input.
    TurnRateGamepad = 45.f;
}

// Called when the game starts or when spawned
void AMyPossessPawn::BeginPlay()
{
	Super::BeginPlay();

    // Find the actor that handles control for the local player.
    OurPlayerController = UGameplayStatics::GetPlayerController(this, 0);
    OriginalPawn = OurPlayerController->GetPawn();
    OriginalCamera = OurPlayerController->GetViewTarget();
}

// Called every frame
void AMyPossessPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    // Handle growing and shrinking based on our "Grow" action
    {
        float CurrentScale = OurVisibleComponent->GetComponentScale().X;
        if (bGrowing)
        {
            // Grow to double size over the course of one second
            CurrentScale += DeltaTime;
        }
        else
        {
            // Shrink half as fast as we grow
            CurrentScale -= (DeltaTime * 0.5f);
        }
        // Make sure we never drop below our starting size, or increase past double size.
        CurrentScale = FMath::Clamp(CurrentScale, 1.0f, 2.0f);
        OurVisibleComponent->SetWorldScale3D(FVector(CurrentScale));
    }

    // Handle movement based on our "MoveX" and "MoveY" and "MoveZ" axes.
    {
        if (!CurrentVelocityForward.IsZero() || !CurrentVelocityRight.IsZero())
        {
            FVector NewLocation = GetActorLocation() + ((CurrentVelocityForward + CurrentVelocityRight) * DeltaTime);
            SetActorLocation(NewLocation);
        }
    }
}

// Called to bind functionality to input
void AMyPossessPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
    
    // Respond when our "Grow" key is pressed or released.
    InputComponent->BindAction("Grow", IE_Pressed, this, &AMyPossessPawn::StartGrowing);
    InputComponent->BindAction("Grow", IE_Released, this, &AMyPossessPawn::StopGrowing);

    // Bind UnPossession event.
    InputComponent->BindAction("Possess", IE_Pressed, this, &AMyPossessPawn::ReleasePawn);

    // Respond every frame to the values of our two movement axes, "MoveX" and "MoveY".
    InputComponent->BindAxis("MoveX", this, &AMyPossessPawn::Move_XAxis);
    InputComponent->BindAxis("MoveY", this, &AMyPossessPawn::Move_YAxis);
    InputComponent->BindAxis("MoveZ", this, &AMyPossessPawn::Move_ZAxis);

    // Mouse rotation copied from PlayerCharacter default code.
    InputComponent->BindAxis("Turn Right / Left Mouse", this, &AMyPossessPawn::AddControllerYawInput);
    InputComponent->BindAxis("Look Up / Down Mouse", this, &AMyPossessPawn::AddControllerPitchInput);
}

void AMyPossessPawn::Move_XAxis(float AxisValue)
{
    // Move along component X axis.
    FVector NewDirection = OurVisibleComponent->GetForwardVector();
    CurrentVelocityForward.X = FMath::Clamp((NewDirection.X * AxisValue), -1.0f, 1.0f) * MovementSpeed;
    CurrentVelocityForward.Y = FMath::Clamp((NewDirection.Y * AxisValue), -1.0f, 1.0f) * MovementSpeed;
}

void AMyPossessPawn::Move_YAxis(float AxisValue)
{
    // Move along component Y axis.
    FVector NewDirection = OurVisibleComponent->GetRightVector();
    CurrentVelocityRight.X = FMath::Clamp((NewDirection.X * AxisValue), -1.0f, 1.0f) * MovementSpeed;
    CurrentVelocityRight.Y = FMath::Clamp((NewDirection.Y * AxisValue), -1.0f, 1.0f) * MovementSpeed;
}

void AMyPossessPawn::Move_ZAxis(float AxisValue)
{
    // Move along component Z axis.
    CurrentVelocityForward.Z = FMath::Clamp(AxisValue, -1.0f, 1.0f) * (MovementSpeed / 2);
}

void AMyPossessPawn::StartGrowing()
{
    bGrowing = true;
}

void AMyPossessPawn::StopGrowing()
{
    bGrowing = false;
}

// Return possession to player character.
void AMyPossessPawn::ReleasePawn()
{
    OurPlayerController->Possess(OriginalPawn);
    FString OriginalName = OriginalPawn->GetName();
    FString DebugMessage = FString::Printf(TEXT("Returning to: %s"), *OriginalName);
    if (GEngine) GEngine->AddOnScreenDebugMessage(INDEX_NONE, 10.f, FColor::Cyan, DebugMessage, true);
}

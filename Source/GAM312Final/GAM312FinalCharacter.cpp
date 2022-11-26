// Copyright Epic Games, Inc. All Rights Reserved.

#include "GAM312FinalCharacter.h"
#include "GAM312FinalProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "Engine.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "TimerManager.h"
#include "CanvasItem.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"

//////////////////////////////////////////////////////////////////////////
// AGAM312FinalCharacter

AGAM312FinalCharacter::AGAM312FinalCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	TurnRateGamepad = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	// Pause menu constructor helper.
	static ConstructorHelpers::FClassFinder<UUserWidget> PauseMenuObj(TEXT("/Game/UI/PauseMenu"));
	PauseWidgetClass = PauseMenuObj.Class;
}

void AGAM312FinalCharacter::BeginPlay()
{
	// Call the base class.
	Super::BeginPlay();

	// Find the actor that handles control for the local player.
	OurPlayerController = UGameplayStatics::GetPlayerController(this, 0);

	// Create pause menu widget.
	PauseWidget = CreateWidget<UUserWidget>(GetWorld(), PauseWidgetClass);

	// Health bar variables.
	FullHealth = 1000.0f;
	Health = FullHealth;
	HealthPercentage = 1.0f;
	PreviousHealth = HealthPercentage;
	redFlash = false;
	SetCanBeDamaged(true);

}

//////////////////////////////////////////////////////////////////////////
// Input

void AGAM312FinalCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind pause event
	PlayerInputComponent->BindAction("PauseMenu", IE_Pressed, this, &AGAM312FinalCharacter::OnPauseMenu);
	
	// Bind fire event
	PlayerInputComponent->BindAction("PrimaryAction", IE_Pressed, this, &AGAM312FinalCharacter::OnPrimaryAction);

	// Bind DisplayRaycast event
	PlayerInputComponent->BindAction("Raycast", IE_Pressed, this, &AGAM312FinalCharacter::DisplayRaycast);

	// Bind Possession event
	PlayerInputComponent->BindAction("Possess", IE_Pressed, this, &AGAM312FinalCharacter::PossessPawn);
	
	// Enable touchscreen input
	EnableTouchscreenMovement(PlayerInputComponent);

	// Bind movement events
	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &AGAM312FinalCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &AGAM312FinalCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "Mouse" versions handle devices that provide an absolute delta, such as a mouse.
	// "Gamepad" versions are for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn Right / Left Gamepad", this, &AGAM312FinalCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("Look Up / Down Gamepad", this, &AGAM312FinalCharacter::LookUpAtRate);
}

void AGAM312FinalCharacter::OnPrimaryAction()
{
	// Trigger the OnItemUsed Event
	OnUseItem.Broadcast();
}

void AGAM312FinalCharacter::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == true)
	{
		return;
	}
	if ((FingerIndex == TouchItem.FingerIndex) && (TouchItem.bMoved == false))
	{
		OnPrimaryAction();
	}
	TouchItem.bIsPressed = true;
	TouchItem.FingerIndex = FingerIndex;
	TouchItem.Location = Location;
	TouchItem.bMoved = false;
}

void AGAM312FinalCharacter::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == false)
	{
		return;
	}
	TouchItem.bIsPressed = false;
}

void AGAM312FinalCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AGAM312FinalCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AGAM312FinalCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void AGAM312FinalCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

bool AGAM312FinalCharacter::EnableTouchscreenMovement(class UInputComponent* PlayerInputComponent)
{
	if (FPlatformMisc::SupportsTouchInput() || GetDefault<UInputSettings>()->bUseMouseForTouch)
	{
		PlayerInputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AGAM312FinalCharacter::BeginTouch);
		PlayerInputComponent->BindTouch(EInputEvent::IE_Released, this, &AGAM312FinalCharacter::EndTouch);

		return true;
	}
	
	return false;
}

// Perform a Raycast (line trace) from the player camera.
void AGAM312FinalCharacter::DisplayRaycast()
{
	// LineTrace input variables.
	float TraceLength = 2000.f;													// Maximum line trace lenghth.
	FVector StartTrace = FirstPersonCameraComponent->GetComponentLocation();	//Start of trace.
	FVector ForwardVector = FirstPersonCameraComponent->GetForwardVector();		//Direction of trace.
	FVector EndTrace = ((ForwardVector * TraceLength) + StartTrace);			//End of trace.
	FCollisionQueryParams* TraceParams = new FCollisionQueryParams();
	// HitResult variables.
	FHitResult* Hitresult = new FHitResult();			//Variable to store Hitresult data.
	float* HitDistance = &Hitresult->Distance;			//Distance to hit.
	FVector* HitLocation = &Hitresult->Location;		//Location of hit (world coords).
	// LineTrace debug display variables.
	FVector TraceTextLocation = ((ForwardVector * 50.f) + StartTrace + FVector(0, 0, -5)); // Trace text location.
	float TraceThickness = 2.f;							// line draw thickness.
	float SphereSize = 6.f;
	FColor TraceColor = FColor::MakeRandomColor();		// Randomized line color for fun!
	bool LinesPersist = false;							// Set to true for permanent line display.
	float TraceDisplayTime = 30.f;						// Maximum display time, if LinesPersist is false.
	
	// Perform line trace.
	if (GetWorld()->LineTraceSingleByChannel(*Hitresult, StartTrace, EndTrace, ECC_Visibility, *TraceParams))
	{
		FString DebugMessage = FString::Printf(TEXT("You hit: %s\nDistance: %.2f"), *Hitresult->GetActor()->GetName(), *HitDistance);
		// Draw line trace debug information to game world.
		DrawDebugSphere(GetWorld(), StartTrace, 0.1f, 12, TraceColor, LinesPersist, TraceDisplayTime, 0U, SphereSize);
		DrawDebugSphere(GetWorld(), *HitLocation, 0.1f, 12, TraceColor, LinesPersist, TraceDisplayTime, 0U, SphereSize);
		DrawDebugLine(GetWorld(), StartTrace, *HitLocation, TraceColor, LinesPersist, TraceDisplayTime, 0U, TraceThickness);
		DrawDebugString(GetWorld(), TraceTextLocation, DebugMessage,0,TraceColor,TraceDisplayTime, true, 1.f);
		// Print hit result to debug console.
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 30.f, TraceColor, DebugMessage, true);
	}
	else
	{
		FColor FailColor = FColor::Black;
		FString DebugMessage = FString::Printf(TEXT("No Hit Within Max\nDistance: %.2f"), TraceLength);
		// Draw line trace debug information to game world.
		DrawDebugSphere(GetWorld(), StartTrace, 0.1f, 12, FailColor, LinesPersist, TraceDisplayTime, 0U, SphereSize);
		DrawDebugLine(GetWorld(), StartTrace, EndTrace, FailColor, LinesPersist, TraceDisplayTime, 0U, TraceThickness);
		DrawDebugString(GetWorld(), TraceTextLocation, DebugMessage, 0, FailColor, TraceDisplayTime, false, 1.f);
		// Print hit result to debug console.
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 30.f, FailColor, DebugMessage, true);
	}
}

// Possess Pawn function.
void AGAM312FinalCharacter::PossessPawn()
{
	// LineTrace input variables.
	float TraceLength = 1000.f;													// Maximum line trace lenghth.
	FVector StartTrace = FirstPersonCameraComponent->GetComponentLocation();	//Start of trace.
	FVector ForwardVector = FirstPersonCameraComponent->GetForwardVector();		//Direction of trace.
	FVector EndTrace = ((ForwardVector * TraceLength) + StartTrace);			//End of trace.
	FCollisionQueryParams* TraceParams = new FCollisionQueryParams();
	// HitResult variables.
	FHitResult* Hitresult = new FHitResult();			//Variable to store Hitresult data.
	APawn* Target;										// Stores pointer to hit actor.
	
	// Perform Line trace.
	if (GetWorld()->LineTraceSingleByChannel(*Hitresult, StartTrace, EndTrace, ECC_Visibility, *TraceParams))
	{
		// Check if we hit APawn, store result to Target.
		Target = Cast<APawn>(Hitresult->GetActor());
		
		if (Target) //If valid target.
		{
			//Possess the Pawn.
			OurPlayerController->Possess(Target);

			//Print debug information.
			FString TargetName = Target->GetName();
			FString DebugMessage = FString::Printf(TEXT("Possessing Pawn : % s"), *TargetName);
			if (GEngine) GEngine->AddOnScreenDebugMessage(INDEX_NONE, 10.f, FColor::Cyan, DebugMessage, true);
		}
	}
}

// HUD Functions.

// Returns health percentage as normalized float value (0-1).
float AGAM312FinalCharacter::GetHealth()
{
	return HealthPercentage;
}

// Returns percentage health value as text (ex. "100%").
FText AGAM312FinalCharacter::GetHealthIntText()
{
	int32 HP = FMath::RoundHalfFromZero(HealthPercentage * 100);
	FString HPS = FString::FromInt(HP);
	FString HealthHUD = HPS + FString(TEXT("%"));
	FText HPText = FText::FromString(HealthHUD);
	return HPText;
}

// Enable damage on player.
void AGAM312FinalCharacter::SetDamageState()
{
	SetCanBeDamaged(true);
}

// Timer for damage delay.
void AGAM312FinalCharacter::DamageTimer()
{
	GetWorldTimerManager().SetTimer(MemberTimerHandle, this, &AGAM312FinalCharacter::SetDamageState, 2.0f, false);
}

// Enable screen flash while being damaged.
bool AGAM312FinalCharacter::PlayFlash()
{
	if (redFlash)
	{
		redFlash = false;
		return true;
	}
	return false;
}

// Apply damage to player.
float AGAM312FinalCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser)
{
	SetCanBeDamaged(false);
	redFlash = true;
	UpdateHealth(-DamageAmount);
	DamageTimer();
	return DamageAmount;
}

// Update health variables.
void AGAM312FinalCharacter::UpdateHealth(float HealthChange)
{
	Health += HealthChange;
	Health = FMath::Clamp(Health, 0.0f, FullHealth);
	PreviousHealth = HealthPercentage;
	HealthPercentage = Health / FullHealth;
}

// Opens pause menu on key press.
void AGAM312FinalCharacter::OnPauseMenu()
{
	// Trigger the PauseMenu Event
	UGameplayStatics::SetGamePaused(GetWorld(), true); //Pause game.
	OurPlayerController->SetInputMode(FInputModeUIOnly()); //Set input to UI only.
	OurPlayerController->SetShowMouseCursor(true); //Show the mouse.
	PauseWidget->AddToViewport(); // Draw pause menu.

}
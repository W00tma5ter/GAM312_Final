// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Components/StaticMeshComponent.h"
#include "MyPossessPawn.generated.h"

UCLASS()
class GAM312FINAL_API AMyPossessPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AMyPossessPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Base turn rate, in deg/sec. Other scaling may affect final turn rate.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float TurnRateGamepad;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere)
	USceneComponent* OurVisibleComponent;

	UCameraComponent* OurCamera;

	// Input functions.
	void Move_XAxis(float AxisValue);
	void Move_YAxis(float AxisValue);
	void Move_ZAxis(float AxisValue);

	void StartGrowing();
	void StopGrowing();

	void ReleasePawn();

	APlayerController* OurPlayerController;
	AActor* OriginalCamera;
	APawn* OriginalPawn;

	// Input variables.
	FVector CurrentVelocityForward;
	FVector CurrentVelocityRight;

	UPROPERTY(EditAnywhere, Category="Movement")
	float MovementSpeed = 250.0f;

	bool bGrowing;

};

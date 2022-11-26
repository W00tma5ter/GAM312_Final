// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Runtime/Engine/Classes/Engine/TargetPoint.h"
#include "MyAIController.generated.h"

//Simple AI controller moves to random waypoints in a navmesh.
UCLASS()
class GAM312FINAL_API AMyAIController : public AAIController
{
	GENERATED_BODY()

public:
    //Override functions.
    void BeginPlay() override;
    void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

private:
    //TimerHandle for delay between moves.
    FTimerHandle DelayTimerHandle;
    
    // Stores array of waypoints.
    UPROPERTY()
        TArray<AActor*> Waypoints; 

    //AI functions.
    UFUNCTION()
        ATargetPoint* GetRandomWaypoint();

    UFUNCTION()
        void GoToRandomWaypoint();
	
};

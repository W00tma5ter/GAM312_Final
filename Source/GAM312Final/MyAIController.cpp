// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAIController.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

void AMyAIController::BeginPlay()
{
    Super::BeginPlay();
    //Find all waypoints in level and store in array.
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATargetPoint::StaticClass(), Waypoints);
    //Take first move.
    GoToRandomWaypoint();
}

// Get random waypoint from list.
ATargetPoint* AMyAIController::GetRandomWaypoint()
{
    auto index = FMath::RandRange(0, Waypoints.Num() - 1);
    return Cast<ATargetPoint>(Waypoints[index]);
}

// Move pawn to a random waypoint.
void AMyAIController::GoToRandomWaypoint() 
{
    MoveToActor(GetRandomWaypoint());
}

// Called once move is completed. Sets timer and moves to another random point.
void AMyAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
    Super::OnMoveCompleted(RequestID, Result);
    
    //Time to wait between moves.
    float WaitTime = 2.0f; 
    //Set Timer, call GoToRandomWaypoint once done, non-looping.
    GetWorldTimerManager().SetTimer(DelayTimerHandle, this, &AMyAIController::GoToRandomWaypoint, WaitTime, false);
}
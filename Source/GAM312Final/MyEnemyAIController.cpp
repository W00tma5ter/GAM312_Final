// Copyright 2022


#include "MyEnemyAIController.h"
#include "NavigationSystem.h"

void AMyEnemyAIController::BeginPlay()
{
    Super::BeginPlay();

    NavArea = FNavigationSystem::GetCurrent<UNavigationSystemV1>(this);
    PatrolRadius = 15000.0f;
    RandomPatrol();
}

void AMyEnemyAIController::RandomPatrol()
{
    if (NavArea)
    {
        NavArea->K2_GetRandomReachablePointInRadius(GetWorld(), GetPawn()->GetActorLocation(), RandomLocation, PatrolRadius);
        MoveToLocation(RandomLocation);
    }
}

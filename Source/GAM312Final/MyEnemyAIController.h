// Copyright 2022

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "MyEnemyAIController.generated.h"

/**
 * 
 */
UCLASS()
class GAM312FINAL_API AMyEnemyAIController : public AAIController
{
	GENERATED_BODY()

    public:
        void BeginPlay() override;

    private:
        class UNavigationSystemV1* NavArea;
        FVector RandomLocation;

    public:
        UPROPERTY(EditAnywhere)
            float PatrolRadius;

        UFUNCTION()
        void RandomPatrol();
	
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyOnHitEvent.generated.h"

UCLASS()
class GAM312FINAL_API AMyOnHitEvent : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyOnHitEvent();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	FString LastHit; //Stores name of last object hit.

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Collission component.
	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* MyComp;

	// Called when Collission componenet registers a hit.
	UFUNCTION()
	void OnCompHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

};

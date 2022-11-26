// Fill out your copyright notice in the Description page of Project Settings.


#include "MyOnHitEvent.h"
#include "Components/BoxComponent.h"
#include "DrawDebugHelpers.h"

// Sets default values
AMyOnHitEvent::AMyOnHitEvent()
{
 	// Set this actor to call Tick() every frame.  [DISABLED]
	PrimaryActorTick.bCanEverTick = false;

	// Create collission component.
	MyComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	MyComp->SetSimulatePhysics(true);
	MyComp->SetNotifyRigidBodyCollision(true);
	MyComp->BodyInstance.SetCollisionProfileName("BlockAllDynamic");
	//Bind Hit function.
	MyComp->OnComponentHit.AddDynamic(this, &AMyOnHitEvent::OnCompHit);
	// Set as root component.
	RootComponent = MyComp;		

}

// Called when the game starts or when spawned
void AMyOnHitEvent::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AMyOnHitEvent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMyOnHitEvent::OnCompHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Check for valid hit, ignore repeat hits.
	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL) && (*OtherActor->GetName() != LastHit))
	{
		LastHit = *OtherActor->GetName(); //Update name of hit actor.
		FString DebugMessage = FString::Printf(TEXT("I Hit: %s"), *LastHit);
			if (GEngine) GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Green, DebugMessage, true); // Print debug info.
		
	}
}


// Fill out your copyright notice in the Description page of Project Settings.


#include "CampFire.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

// Sets default values
ACampFire::ACampFire()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    MyBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("My Box Component"));
    MyBoxComponent->InitBoxExtent(FVector(50.0f, 50.0f, 50.0f));
    RootComponent = MyBoxComponent;

    Fire = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("My Fire"));
    Fire->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
    Fire->SetupAttachment(RootComponent);

    MyBoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ACampFire::OnOverlapBegin);
    MyBoxComponent->OnComponentEndOverlap.AddDynamic(this, &ACampFire::OnOverlapEnd);

    bCanApplyDamage = false;

}

// Called when the game starts or when spawned
void ACampFire::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACampFire::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// While player is overlapping, apply damage every timer cycle.
void ACampFire::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
    {
        bCanApplyDamage = true;
        MyCharacter = Cast<AActor>(OtherActor);
        MyHit = SweepResult;
        GetWorldTimerManager().SetTimer(FireTimerHandle, this, &ACampFire::ApplyFireDamage, 2.2f, true, 0.0f);
    }
}

// When player leaves, stop timer and disable damage effect.
void ACampFire::OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    bCanApplyDamage = false;
    GetWorldTimerManager().ClearTimer(FireTimerHandle);
}

// Apply damage to successful overlapping actor.
void ACampFire::ApplyFireDamage()
{
    FPointDamageEvent DamageEvent;
    DamageEvent.HitInfo = MyHit;

    if (bCanApplyDamage)
    {
        MyCharacter->TakeDamage(200.0f, DamageEvent, MyCharacter->GetInstigatorController(), this);
    }
}

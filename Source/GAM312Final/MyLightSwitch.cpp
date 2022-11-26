// Fill out your copyright notice in the Description page of Project Settings.


#include "MyLightSwitch.h"
#include "Components/PointLightComponent.h"
#include "Components/SphereComponent.h"
#include "DrawDebugHelpers.h"

// Sets default values
AMyLightSwitch::AMyLightSwitch()
{
 	// Set this actor to call Tick() every frame.  [DISABLED]
	PrimaryActorTick.bCanEverTick = false;

	// Create point light.
	LightIntensity = 3000.0f;
	PointLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("Point Light"));
	PointLight->Intensity = LightIntensity;
	PointLight->bAddedToSceneVisible = true;
	PointLight->SetVisibility(false);
	RootComponent = PointLight;

	// Create collission volume.
	LightSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Light Sphere Component"));
	LightSphere->InitSphereRadius(300.0f);
	LightSphere->SetCollisionProfileName(TEXT("Trigger Volume"));
	LightSphere->SetupAttachment(RootComponent);

	// Create overlap events.
	LightSphere->OnComponentBeginOverlap.AddDynamic(this, &AMyLightSwitch::OnOverlapBegin);
	LightSphere->OnComponentEndOverlap.AddDynamic(this, &AMyLightSwitch::OnOverlapEnd);
}

// Called when the game starts or when spawned
void AMyLightSwitch::BeginPlay()
{
	Super::BeginPlay();

	// Draw debug sphere representing collission volume.
	DrawDebugSphere(GetWorld(), LightSphere->GetComponentLocation(), LightSphere->GetScaledSphereRadius(), 50, FColor::Green, true, -1, 0, 2);
}

// Called every frame
void AMyLightSwitch::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

//Overlap Begin Function.
void AMyLightSwitch::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		// Turn on light.
		PointLight->SetVisibility(true);
		if (GEngine) GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Green, TEXT("Light ON"), true); // Print debug info.
	}
}
//Overlap End Function.
void AMyLightSwitch::OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		// Turn off light.
		PointLight->SetVisibility(false);
		if (GEngine) GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Green, TEXT("Light OFF"), true); // Print debug info.
	}
}

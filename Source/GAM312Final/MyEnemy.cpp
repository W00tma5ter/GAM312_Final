// Copyright 2022


#include "MyEnemy.h"
#include "GAM312FinalCharacter.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "MyEnemyAIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "AITypes.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"

// Sets default values
AMyEnemy::AMyEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PlayerCollisionDetection = CreateDefaultSubobject<USphereComponent>(TEXT("Player Collision Detection"));
	PlayerCollisionDetection->SetupAttachment(RootComponent);

	PlayerAttackCollisionDetection = CreateDefaultSubobject<USphereComponent>(TEXT("Player Attack Collision Detection"));
	PlayerAttackCollisionDetection->SetupAttachment(RootComponent);

	DamageCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Damage Collision"));
	DamageCollision->SetupAttachment(GetMesh(), TEXT("RightHandSocket"));

}

// Called when the game starts or when spawned
void AMyEnemy::BeginPlay()
{
	Super::BeginPlay();

	AIControllerREF = Cast<AMyEnemyAIController>(GetController());
	AIControllerREF->GetPathFollowingComponent()->OnRequestFinished.AddUObject(this, &AMyEnemy::OnAIMoveCompleted);

	PlayerCollisionDetection->OnComponentBeginOverlap.AddDynamic(this, &AMyEnemy::OnPlayerDetectedOverlapBegin);

	PlayerCollisionDetection->OnComponentEndOverlap.AddDynamic(this, &AMyEnemy::OnPlayerDetectedOverlapEnd);

	PlayerAttackCollisionDetection->OnComponentBeginOverlap.AddDynamic(this, &AMyEnemy::OnPlayerAttackOverlapBegin);

	PlayerAttackCollisionDetection->OnComponentEndOverlap.AddDynamic(this, &AMyEnemy::OnPlayerAttackOverlapEnd);

	DamageCollision->OnComponentBeginOverlap.AddDynamic(this, &AMyEnemy::OnDealDamageOverlapBegin);

	AnimInstance = GetMesh()->GetAnimInstance();
	
}

// Called every frame
void AMyEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMyEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AMyEnemy::OnAIMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	if (!PlayerDetected)
	{
		AIControllerREF->RandomPatrol();
	}
	else if (PlayerDetected && CanAttackPlayer)
	{
		StopSeekingPlayer();

		// attack player
		AnimInstance->Montage_Play(EnemyAttackAnimation);
	}
}

void AMyEnemy::MoveToPlayer()
{
	PlayerREF = UGameplayStatics::GetPlayerCharacter(this, 0);
	AIControllerREF->MoveToLocation(PlayerREF->GetActorLocation(), StoppingDistance, true);
}

void AMyEnemy::SeekPlayer()
{
	MoveToPlayer();
	GetWorld()->GetTimerManager().SetTimer(SeekPlayerTimerHandle, this, &AMyEnemy::SeekPlayer, 0.25f, true);
}

void AMyEnemy::StopSeekingPlayer()
{
	GetWorld()->GetTimerManager().ClearTimer(SeekPlayerTimerHandle);
}

void AMyEnemy::OnPlayerDetectedOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	PlayerREF = Cast<AGAM312FinalCharacter>(OtherActor);
	
	if (PlayerREF)
	{
		PlayerDetected = true;
		SeekPlayer();
	}
}

void AMyEnemy::OnPlayerDetectedOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	PlayerREF = Cast<AGAM312FinalCharacter>(OtherActor);
	
	if (PlayerREF)
	{
		PlayerDetected = false;
		StopSeekingPlayer();
		AIControllerREF->RandomPatrol();
	}
}

void AMyEnemy::OnPlayerAttackOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	PlayerREF = Cast<AGAM312FinalCharacter>(OtherActor);
	
	if (PlayerREF)
	{
		CanAttackPlayer = true;
	}
}

void AMyEnemy::OnPlayerAttackOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	PlayerREF = Cast<AGAM312FinalCharacter>(OtherActor);
	
	if (PlayerREF)
	{
		CanAttackPlayer = false;

		// stop the attack animation and chase the player
		AnimInstance->Montage_Stop(0.0f, EnemyAttackAnimation);

		SeekPlayer();
	}
}

void AMyEnemy::OnDealDamageOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	PlayerREF = Cast<AGAM312FinalCharacter>(OtherActor);
	
	if (PlayerREF && CanDealDamage && Cast<AGAM312FinalCharacter>(OtherActor))
	{
		FPointDamageEvent DamageEvent;

		//Punch player until they reach target health percentage. Punching does not kill if value > 0.
		if (Cast<AGAM312FinalCharacter>(OtherActor)->GetHealth() >= 0.25f)
		{
			// deal damage to player
			//UE_LOG(LogTemp, Warning, TEXT("Player Damaged"));
			OtherActor->TakeDamage(50.0f, DamageEvent, OtherActor->GetInstigatorController(), this);
		}
	}
}

void AMyEnemy::AttackAnimationEnded()
{
	if (CanAttackPlayer)
	{
		AnimInstance->Montage_Play(EnemyAttackAnimation);
	}
}


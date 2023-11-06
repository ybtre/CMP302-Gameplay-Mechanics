// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyA.h"
#include "AIController.h"
#include "Macros.h"
#include "MainCharacter.h"
#include "MainPlayerController.h"
#include "TimerManager.h"
#include "Animation/AnimInstance.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"


// Sets default values
AEnemyA::AEnemyA()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AggroSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AggroSphere"));
	AggroSphere->SetupAttachment(GetRootComponent());
	AggroSphere->InitSphereRadius(700.f);

	CombatSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CombatSphere"));
	CombatSphere->SetupAttachment(GetRootComponent());
	CombatSphere->InitSphereRadius(220.f);

	
	InterpSpeed = 15.f;
	bInterpToTarget = false;
	
	AttackHitbox = CreateDefaultSubobject<UBoxComponent>("AttackHitbox");
	AttackHitbox->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale,
		FName("AttackHitboxSocket"));

	
	//
	bOverlappingCombatSphere = false;
	bAttacking = false;

	MaxHealth = 100.f;
	Health = 75.f;
	Damage = 10.f;

	AttackMinTime = 0.5f;
	AttackMaxTime = 2.5f;

	SetEnemyMovementStatus(EEnemyMovementStatus::EMS_IDLE);
	CorpseLifespan = 3.f;
	
}

// Called when the game starts or when spawned
void AEnemyA::BeginPlay()
{
	Super::BeginPlay();

	AIController = Cast<AAIController>(GetController());

	AggroSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemyA::AggroSphere_OnOverlapBegin);
	AggroSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemyA::AggroSphere_OnOverlapEnd);

	CombatSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemyA::CombatSphere_OnOverlapBegin);
	CombatSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemyA::CombatSphere_OnOverlapEnd);


	AttackHitbox->OnComponentBeginOverlap.AddDynamic(this, &AEnemyA::AttackHOnOverlapBegin);
	AttackHitbox->OnComponentEndOverlap.AddDynamic(this, &AEnemyA::AttackHOnOverlapEnd);
	
	//
	// query only sets it to not use physics, only to overlap and generate overlap events
	AttackHitbox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AttackHitbox->SetCollisionObjectType(ECC_WorldDynamic);
	AttackHitbox->SetCollisionResponseToAllChannels(ECR_Ignore);					// first ignore everything
	AttackHitbox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);	// then set to only respond to PAWN objects
}

// Called every frame
void AEnemyA::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if ( bInterpToTarget && InterpTarget) {
		FRotator LookAtYaw = GetLookAtRotationYaw(InterpTarget->GetActorLocation());
		FRotator InterpRotation = FMath::RInterpTo(GetActorRotation(), LookAtYaw, DeltaTime, InterpSpeed);

		SetActorRotation(InterpRotation);
	}
}

// Called to bind functionality to input
void AEnemyA::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemyA::SetInterpToPlayerTarget(bool Interp) {
	bInterpToTarget = Interp;
}

FRotator AEnemyA::GetLookAtRotationYaw(FVector Target) {
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target);
	FRotator LookAtRotationYaw(0.f, LookAtRotation.Yaw, 0.f);

	return LookAtRotationYaw;
}

void AEnemyA::AggroSphere_OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                         UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {

		if (OtherActor && Alive()) {
			
			AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor);
			if (MainCharacter) {
				MoveToTarget(MainCharacter);
			}
		}
}

void AEnemyA::AggroSphere_OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {

	if (OtherActor) {

		AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor);
		if (MainCharacter) {
			if (MainCharacter->CombatTarget == this) {
				MainCharacter->SetInterpCombatTarget(false);
				MainCharacter->SetHasCombatTarget(false);
			}
			
			if(MainCharacter->MainPlayerController) {
				MainCharacter->MainPlayerController->RemoveEnemyHealthBar();
			}
			
			SetEnemyMovementStatus(EEnemyMovementStatus::EMS_IDLE);
			if (AIController) {
				AIController->StopMovement();
			}
		}
	}
}

void AEnemyA::CombatSphere_OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {

	if (OtherActor && Alive()) {
		
		AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor);
		if (MainCharacter) {
			if(!bAttacking) {
				MainCharacter->SetInterpCombatTarget(this);
				MainCharacter->SetHasCombatTarget(true);
				if (MainCharacter->MainPlayerController) {
					MainCharacter->MainPlayerController->DisplayEnemyHealthBar();
				}
				
				CombatTarget = MainCharacter;
			
				bOverlappingCombatSphere = true;
				SetInterpToPlayerTarget(true);
			}

			Attack();
		}
	}
}

void AEnemyA::CombatSphere_OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {

	if (OtherActor) {
		
		AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor);
		if (MainCharacter) {
			bOverlappingCombatSphere = false;
			SetInterpToPlayerTarget(false);
 
			SetEnemyMovementStatus(EEnemyMovementStatus::EMS_MoveToTarget);
			MoveToTarget(MainCharacter);
			CombatTarget = nullptr;
		}
		
		GetWorldTimerManager().ClearTimer(AttackTimer);
	}
}

void AEnemyA::AttackHOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	
	if (OtherActor && Alive()) {

		AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor);
		if (MainCharacter) {
			if (DamageTypeClass) {
				// ApplyDamage calls the MainCharacter's TakeDamage function
				UGameplayStatics::ApplyDamage(MainCharacter, Damage, AIController, this, DamageTypeClass);
			}
		}
	}
}


void AEnemyA::AttackHOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	
	UE_LOG(LogTemp, Warning, TEXT("ATTACKHITBOX_OVERLAPEND"));
}

void AEnemyA::MoveToTarget(AMainCharacter* Target) {
	if(Alive()) {
		SetEnemyMovementStatus(EEnemyMovementStatus::EMS_MoveToTarget);

		if (AIController) {
			///////////////////////////////////////
			//             easy way				 //
			AIController->MoveToActor(Target);
			///////////////////////////////////////
		
			/* harder way that provides information about the pathing from the *NavPath
			* also provides more customization about the MoveTo...()
			*/
			/*FAIMoveRequest MoveRequest;
			MoveRequest.SetGoalActor(Target);
			MoveRequest.SetAcceptanceRadius(5.f); // distance between target and object where the movement will stop

			/* NavPath is an OutPath, meaning the outparam of the function.
			* The NavPath will be filled with the path information by the function. Not by me
			#1#
			FNavPathSharedPtr NavPath; 

			AIController->MoveTo(MoveRequest, &NavPath);

			auto PathPoints = NavPath->GetPathPoints();
			for ( auto Point : PathPoints) {
			FVector Location = Point.Location;

			UKismetSystemLibrary::DrawDebugSphere(this,Location,15,12,
			FLinearColor::Green,10,2);
			}*/
		}
	}
}

void AEnemyA::EnableCollision() {
	// UE_LOG(LogTemp, Warning, TEXT("Enable COllision"));
	DS("Enemy Enable Disable Collision");
	AttackHitbox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AEnemyA::DeactivateCollision() {
	// UE_LOG(LogTemp, Warning, TEXT("Disable COllision"));
	DS("Enemy Enable Disable Collision");
	AttackHitbox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemyA::Attack() {
	if (Alive()) {
		if (AIController) {
			AIController->StopMovement();
			SetEnemyMovementStatus(EEnemyMovementStatus::EMS_ATTACKING);
		}

		if (!bAttacking) {
			bAttacking = true;
			if(bAttacking) {
				GetCharacterMovement()->MaxWalkSpeed = 0;
			}

			// TODO: Set enemy to interp to player on every new attack while the player is in the combat sphere
			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
			if (AnimInstance) {
				AnimInstance->Montage_Play(CombatMontage, 1.4f);
				AnimInstance->Montage_JumpToSection(FName("Attack"), CombatMontage);
			}
		}
	}
}

void AEnemyA::AttackEnd() {
	bAttacking = false;
	if(!bAttacking) {
		GetCharacterMovement()->MaxWalkSpeed = 300;
	}
	
	if (bOverlappingCombatSphere) {
		float AttackTime = FMath::FRandRange(AttackMinTime, AttackMaxTime);
		GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemyA::Attack, AttackTime);
	}
}

float AEnemyA::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
    AActor* DamageCauser) {

	if (Health - DamageAmount <= 0) {
		Health -= DamageAmount;
		Die();
	} else {
		Health -= DamageAmount;
	}

	return DamageAmount;
}

void AEnemyA::DecrementHealth(float DmgAmount) {
	if (Health - DmgAmount <= 0) {
		Health -= DmgAmount;
	} else {
		Health -= DmgAmount;
	}
}

void AEnemyA::Die() {
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && CombatMontage) {
		AnimInstance->Montage_Play(CombatMontage, 1.2f);
		AnimInstance->Montage_JumpToSection(FName("Death"), CombatMontage);
		//ExoGame_Bears_Attack_Death_2
	}
	SetEnemyMovementStatus(EEnemyMovementStatus::EMS_DEAD);

	AggroSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AttackHitbox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemyA::DeathEndAnim() {
	GetMesh()->bPauseAnims = true;
	GetMesh()->bNoSkeletonUpdate = true;
	
	GetWorldTimerManager().SetTimer(DeathTimer, this, &AEnemyA::CorpseDisappear, CorpseLifespan);
}

bool AEnemyA::Alive() {
	return GetEnemyMovementStatus() != EEnemyMovementStatus::EMS_DEAD;
}

void AEnemyA::CorpseDisappear() {
	Destroy();
}

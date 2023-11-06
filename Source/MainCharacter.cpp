// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacter.h"
#include "EnemyA.h"
#include "Macros.h"
#include "MainPlayerController.h"
#include "MainPlayerController.h"
#include "Weapon.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/DecalComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Particles/ParticleSystemComponent.h"


// Sets default values
AMainCharacter::AMainCharacter() {
    // Activate ticking in order to update the cursor every frame.
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = true;

    // dont rotate when the controller rotates
    // let that jsut affect the camera
    bUseControllerRotationYaw = false;
    bUseControllerRotationPitch = false;
    bUseControllerRotationRoll = false;

    // configure character movement
    GetCharacterMovement()->bOrientRotationToMovement = true; // character moves in the direction of input...
    GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f); // ... at this rotation rate
    GetCharacterMovement()->bConstrainToPlane = true;
    GetCharacterMovement()->bSnapToPlaneAtStart = true;

    // Create Camera Boom
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(GetRootComponent());
    CameraBoom->SetUsingAbsoluteRotation(true); // dont want the arm to rotate when the character does
    CameraBoom->TargetArmLength = 1100.f; // camera follows at this distance
    CameraBoom->SetRelativeRotation(FRotator(-50, 0, 0));
    CameraBoom->bDoCollisionTest = false; // dont pull in camera when it collides with level

    // Create Top Down Follow Player Camera
    TopDownFollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownFollowCamera"));
    TopDownFollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
    TopDownFollowCamera->bUsePawnControlRotation = false;

    // set size for collision capsule

    // create decal in the world to show the cursor's location
    CursorToWorld = CreateDefaultSubobject<UDecalComponent>("CursorToWorld");
    CursorToWorld->SetupAttachment(GetRootComponent());
    static ConstructorHelpers::FObjectFinder<UMaterial> DecalMaterialAsset(
        TEXT("Material'/Game/TopDownCPP/Blueprints/M_Cursor_Decal.M_Cursor_Decal'"));
    if (DecalMaterialAsset.Succeeded()) {
        UE_LOG(LogTemp, Warning, TEXT("Successfully Loaded Cursor Decal"));
        CursorToWorld->SetDecalMaterial(DecalMaterialAsset.Object);
    }
    else {
        UE_LOG(LogTemp, Error, TEXT("Could NOT load cursor decal"));
    }
    CursorToWorld->DecalSize = FVector(16, 32, 32);
    CursorToWorld->SetRelativeRotation(FRotator(90, 0, 0).Quaternion());

    InterpSpeed = 15.f;
    bInterpToEnemy = false;

    GetCapsuleComponent()->InitCapsuleSize(54, 104);

    // attach the sword hitbox to the FX_Sword_Top socket on the player mesh
    SwordHitbox = CreateDefaultSubobject<UBoxComponent>(TEXT("SwordHitbox"));
    SwordHitbox->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale,
                                   FName("FX_Sword_Top"));

    // timers
    fElapsedTime_ManaPotion = 0;
    fElapsedTime_RockBurstCooldown = 0;
    
    //
    //////////////////
    // Vortex Stats //
    //////////////////
    SetVortexAbility_Damage(.35f);
    SetVortexAbility_InitialAoE(FVector(2, 2, 0));
    SetVortexAbility_CurrentAoE(230.f);
    SetVortexAbility_MaxStacks(10);
    SetVortexAbility_CurrentStacks(1);
    SetVortexAbility_InitialManaCost(20.f);
    SetVortexAbility_ManaDrain(15.f);

    VortexAbility_Hitbox = CreateDefaultSubobject<USphereComponent>(TEXT("VortexHitbox"));
    VortexAbility_Hitbox->SetupAttachment(GetRootComponent());
    VortexAbility_Hitbox->InitSphereRadius(270);

    VortexParticleComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("VortexPart"));
    VortexParticleComponent->SetupAttachment(VortexAbility_Hitbox);
    VortexParticleComponent->SetRelativeScale3D(FVector(0.5));
    VortexParticleComponent->bAutoActivate = false;
    VortexParticleComponent->SetUsingAbsoluteRotation(true);
    VortexParticleComponent->DeactivateSystem();

    //
    //////////////////
    // RockBurst Stats //
    //////////////////
    SetRockBurstAbility_Damage(100.f);
    SetRockBurstAbility_CurrentAoE(230.f);
    SetRockBurstAbility_InitialManaCost(50.f);
    SetRockBurstAbility_Cooldown(8.f);


    RockBurstAbility_Hitbox = CreateDefaultSubobject<USphereComponent>(TEXT("RockBurstHitbox"));
    RockBurstAbility_Hitbox->SetupAttachment(GetRootComponent());
    RockBurstAbility_Hitbox->InitSphereRadius(270);

    RockBurstParticleComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("RockBurstParticle"));
    RockBurstParticleComponent->SetupAttachment(RockBurstAbility_Hitbox);
    RockBurstParticleComponent->SetRelativeScale3D(FVector(0.8));
    RockBurstParticleComponent->bAutoActivate = false;
    RockBurstParticleComponent->SetUsingAbsoluteRotation(true);
    RockBurstParticleComponent->DeactivateSystem();

    //
    //////////////////
    // Player Stats //
    //////////////////
    SetMaxHealth(100.f);
    SetCurrentHealth(65.f);
    SetMaxMana(200.f);
    SetCurrentMana(150.f);
    SetManaRegen(10.f);
    SetCurrentWeaponDmg(15.f);
    SetPlayerMovementSpeed(600.f);
    SetPlayerMovement_ChannelingSpeed(450.f);
    SetManaStatus(EManaRegenStatus::EMRS_NORMAL);
    SetManaRegenBuffDuration(2.f);

    bIsDead = false; // me
    bHasCombatTarget = false;
    bCanCast_RockBurst = true;

    // input button/mouse down booleans
    bLMBDown = false;
    bQKeyDown = false;
    bWKeyDown = false;
}

// Called when the game starts or when spawned
void AMainCharacter::BeginPlay() {
    Super::BeginPlay();

    MainPlayerController = Cast<AMainPlayerController>(GetController());

    SwordHitbox->OnComponentBeginOverlap.AddDynamic(this, &AMainCharacter::Attack_OnOverlapBegin);
    SwordHitbox->OnComponentEndOverlap.AddDynamic(this, &AMainCharacter::Attack_OnOverlapEnd);

    // query only sets it to not use physics, only to overlap and generate overlap events
    SwordHitbox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    SwordHitbox->SetCollisionObjectType(ECC_WorldDynamic);
    SwordHitbox->SetCollisionResponseToAllChannels(ECR_Ignore); // first ignore everything
    SwordHitbox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

    VortexAbility_Hitbox->OnComponentBeginOverlap.AddDynamic(this, &AMainCharacter::Vortex_OnOverlapBegin);
    VortexAbility_Hitbox->OnComponentEndOverlap.AddDynamic(this, &AMainCharacter::Vortex_OnOverlapEnd);

    VortexAbility_Hitbox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    VortexAbility_Hitbox->SetCollisionObjectType(ECC_WorldDynamic);
    VortexAbility_Hitbox->SetCollisionResponseToAllChannels(ECR_Ignore);
    VortexAbility_Hitbox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

    RockBurstAbility_Hitbox->OnComponentBeginOverlap.AddDynamic(this, &AMainCharacter::RockBurst_OnOverlapBegin);
    RockBurstAbility_Hitbox->OnComponentEndOverlap.AddDynamic(this, &AMainCharacter::RockBurst_OnOverlapEnd);

    RockBurstAbility_Hitbox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    RockBurstAbility_Hitbox->SetCollisionObjectType(ECC_WorldDynamic);
    RockBurstAbility_Hitbox->SetCollisionResponseToAllChannels(ECR_Ignore);
    RockBurstAbility_Hitbox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

// Called every frame
void AMainCharacter::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);

    FString TheFloatStr = FString::SanitizeFloat(fElapsedTime_RockBurstCooldown);
    GEngine->AddOnScreenDebugMessage( -1,1.0,FColor::Red, *TheFloatStr );
    
    if (CursorToWorld) {
        if (AMainPlayerController* PlayerController = Cast<AMainPlayerController>(GetController())) {
            FHitResult TraceHitResult;
            PlayerController->GetHitResultUnderCursor(ECC_Visibility, true, TraceHitResult);
            FVector CursorForwardVector = TraceHitResult.ImpactNormal;
            FRotator CursorRotator = CursorForwardVector.Rotation();
            CursorToWorld->SetWorldLocation(TraceHitResult.Location);
            CursorToWorld->SetWorldRotation(CursorRotator);
        }
    }

    if (GetCurrentMana() < 0) {
        SetCurrentMana(0);
    }
    if(GetCurrentHealth() > GetMaxHealth()) {
        SetCurrentHealth(GetMaxHealth());
    }

    if (bQKeyDown && GetCurrentMana() > GetVortexAbility_InitialManaCost()) {
        VortexParticleComponent->SetWorldLocation(GetActorLocation());

        if (bVortexDealDmg) {
            UGameplayStatics::ApplyDamage(DamagedEnemyByVortex, GetVortexAbility_Damage(), GetInstigatorController(),
                                          this,
                                          DamageTypeClass);
        }
    }
    else if (bQKeyDown && GetCurrentMana() <= GetVortexAbility_InitialManaCost()) {
        QKeyUp();
    }

    if(!bCanCast_RockBurst) {
        if(fElapsedTime_RockBurstCooldown >= GetRockBurstAbility_Cooldown()) {
            bCanCast_RockBurst = true;
        } else {
          fElapsedTime_RockBurstCooldown += DeltaTime;
        }
    }
    
    if (bWKeyDown && GetCurrentMana() > GetRockBurstAbility_InitialManaCost()) {
        if(bCanCast_RockBurst) {
            RockBurstParticleComponent->SetWorldLocation(GetActorLocation());

            if(bRockBurstDealDmg) {
                UGameplayStatics::ApplyDamage(DamagedEnemyByRockBurst, GetRockBurstAbility_Damage(), GetInstigatorController(),
                    this, DamageTypeClass);
                bRockBurstDealDmg = false;
            }

            bCanCast_RockBurst = false;
            fElapsedTime_RockBurstCooldown = 0;
        } 
    } else if(bWKeyDown && GetCurrentMana() <= GetRockBurstAbility_InitialManaCost()) {
        WKeyUp();
    }

    float VortexManaDrainPerSecond = GetVortexAbility_ManaDrain() * DeltaTime;
    float regenRate = GetCurrentMana() + GetManaRegen() * DeltaTime;
    switch (ManaStatus) {
    case EManaRegenStatus::EMRS_NORMAL:
        if (GetCurrentMana() <= GetMaxMana()) {
            SetCurrentMana(regenRate);
        }
        else {
            SetCurrentMana(GetMaxMana());
        }

        if (bQKeyDown) {
            if (GetCurrentMana() <= GetVortexAbility_InitialManaCost()) {
                DS("Not Enough Mana To Cast Vortex Ability");
                VortexParticleComponent->KillParticlesForced();
                bQKeyDown = false;
            }
            else {
                float newManaAmount = regenRate - VortexManaDrainPerSecond;
                SetCurrentMana(newManaAmount);
            }
        }

        if(bWKeyDown) {
            if(GetCurrentMana() <= GetRockBurstAbility_InitialManaCost()) {
                DS("Not Enough Mana to Cast Rock Burst Ability");
                RockBurstParticleComponent->KillParticlesForced();
                bWKeyDown = false;
            } 
        }
        break;
    case EManaRegenStatus::EMRS_POTION:
        if(fElapsedTime_ManaPotion < GetManaRegenBuffDuration()) {
            fElapsedTime_ManaPotion += DeltaTime;

            regenRate = GetCurrentMana() + GetManaRegen() * 5 * DeltaTime;
            if (GetCurrentMana() <= GetMaxMana()) {
                SetCurrentMana(regenRate);
            }
            else {
                SetCurrentMana(GetMaxMana());
            }

            if (bQKeyDown) {
                if (GetCurrentMana() <= GetVortexAbility_InitialManaCost()) {
                    DS("Not Enough Mana To Cast Vortex Ability");
                    QKeyUp();
                }
                else {
                    float newManaAmount = regenRate - VortexManaDrainPerSecond;
                    SetCurrentMana(newManaAmount);
                }
            }

            if(bWKeyDown) {
                if(GetCurrentMana() <= GetRockBurstAbility_InitialManaCost()) {
                    DS("Not Enough Mana To Cast Rock Burst Ability");
                    WKeyUp();
                } 
            }
            
        } else if(fElapsedTime_ManaPotion >= GetManaRegenBuffDuration()) {
            SetManaStatus(EManaRegenStatus::EMRS_NORMAL);
            fElapsedTime_ManaPotion = 0;
        }
        break;
    default: ;
    }

    if (bInterpToEnemy && CombatTarget) {
        FRotator LookAtYaw = GetLookAtRotationYaw(CombatTarget->GetActorLocation());
        FRotator InterpRotation = FMath::RInterpTo(GetActorRotation(), LookAtYaw, DeltaTime, InterpSpeed);

        SetActorRotation(InterpRotation);
    }
    if (CombatTarget) {
        CombatTargetLocation = CombatTarget->GetActorLocation();
        if (MainPlayerController) {
            MainPlayerController->EnemtLocation = CombatTargetLocation;
        }
    }

}

// Called to bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    check(PlayerInputComponent)

    PlayerInputComponent->BindAction("LMB", IE_Pressed, this, &AMainCharacter::LMBDown);
    PlayerInputComponent->BindAction("LMB", IE_Released, this, &AMainCharacter::LMBUp);

    PlayerInputComponent->BindAction("Ability1", IE_Pressed, this, &AMainCharacter::QKeyDown);
    PlayerInputComponent->BindAction("Ability1", IE_Released, this, &AMainCharacter::QKeyUp);

    PlayerInputComponent->BindAction("Ability2", IE_Pressed, this, &AMainCharacter::WKeyDown);
    PlayerInputComponent->BindAction("Ability2", IE_Released, this, &AMainCharacter::WKeyUp);

}

FRotator AMainCharacter::GetLookAtRotationYaw(FVector Target) {
    FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target);
    FRotator LookAtRotationYaw(0.f, LookAtRotation.Yaw, 0.f);

    return LookAtRotationYaw;
}

void AMainCharacter::LMBDown() {

    bLMBDown = true;

    if (bIsDead) { return; }

    if (ActiveOverlappingItem) {

        AWeapon* Weapon = Cast<AWeapon>(ActiveOverlappingItem);
        if (Weapon) {
            Weapon->Equip(this);
            SetActiveOverlappingItem(nullptr);
            // set/update to nullptr after it has been set so that we can overlap with other items as well
        }
    }
    else if (EquippedWeapon) {
        Attack();
    }
    else {
        Attack();
    }

}

void AMainCharacter::LMBUp() {

    bLMBDown = false;
}

void AMainCharacter::QKeyDown() {

    bQKeyDown = true;
    if (bIsDead) { return; }

    if (bQKeyDown && GetCurrentMana() > GetVortexAbility_InitialManaCost()) {
        SetCurrentMana(GetCurrentMana() - GetVortexAbility_InitialManaCost());

        if (bQKeyDown) {
            GetCharacterMovement()->MaxWalkSpeed = GetPlayerMovement_ChannelingSpeed();
            UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), VortexTemplate, GetActorLocation(), FRotator(0.f),
                                                     true);
            if (!VortexParticleComponent->IsActive()) {
                VortexParticleComponent->ActivateSystem(true);
            }
        }
        else {
            GetCharacterMovement()->MaxWalkSpeed = GetPlayerMovementSpeed();
        }
    }
    else if (bQKeyDown && GetCurrentMana() <= GetVortexAbility_InitialManaCost()) {
          QKeyUp();
    }
}

void AMainCharacter::QKeyUp() {

    bQKeyDown = false;
    if (!bQKeyDown) {
        GetCharacterMovement()->MaxWalkSpeed = GetPlayerMovementSpeed();
        VortexParticleComponent->KillParticlesForced();
        VortexParticleComponent->DeactivateSystem();
    }
    else {
        GetCharacterMovement()->MaxWalkSpeed = GetPlayerMovement_ChannelingSpeed();
    }
}

void AMainCharacter::WKeyDown() {
    bWKeyDown = true;
    if (bIsDead) { return; }

    if(bCanCast_RockBurst) {
        if (bWKeyDown && GetCurrentMana() > GetRockBurstAbility_InitialManaCost()) {
            SetCurrentMana(GetCurrentMana() - GetRockBurstAbility_InitialManaCost());

            if (bWKeyDown) {
                UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), RockBurstTemplate, GetActorLocation(), FRotator(0.f),
                                                         true);
                if (!RockBurstParticleComponent->IsActive()) {
                    RockBurstParticleComponent->ActivateSystem(true);
                }
            }
        }
    }
    else if (bWKeyDown && GetCurrentMana() <= GetRockBurstAbility_InitialManaCost()) {
        WKeyUp();
    }
}

void AMainCharacter::WKeyUp() {
    bWKeyDown = false;
    if (!bWKeyDown) {
        GetCharacterMovement()->MaxWalkSpeed = GetPlayerMovementSpeed();
        VortexParticleComponent->KillParticlesForced();
        VortexParticleComponent->DeactivateSystem();
    }
}

void AMainCharacter::SetInterpToEnemy(bool Interp) {
    bInterpToEnemy = Interp;
}

void AMainCharacter::SetEquippedWeapon(AWeapon* WeaponToEquip) {

    if (EquippedWeapon) {
        EquippedWeapon->Destroy();
    }

    EquippedWeapon = WeaponToEquip;
}

void AMainCharacter::Attack_OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                           UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                           const FHitResult& SweepResult) {

    if (OtherActor) {

        AEnemyA* EnemyA = Cast<AEnemyA>(OtherActor);
        if (EnemyA) {
            EnemyA->SetInterpTarget(this);
        }
        if (DamageTypeClass) {
            UGameplayStatics::ApplyDamage(EnemyA, GetCurrentWeaponDmg(), GetInstigatorController(), this,
                                          DamageTypeClass);
        }
    }
}

void AMainCharacter::Attack_OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                         UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {

    if (OtherActor) {

        AEnemyA* EnemyA = Cast<AEnemyA>(OtherActor);
        if (EnemyA) {
            EnemyA->SetInterpTarget(nullptr);
        }
    }
}

void AMainCharacter::Attack() {

    if (!bAttacking && !bIsDead) {
        SetBAttacking(true);
        SetInterpToEnemy(true);

        UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
        if (AnimInstance && CombatMontageA && CombatMontageB) {

            int32 Section = FMath::RandRange(0, 1);
            switch (Section) {
            case 0:
                AnimInstance->Montage_Play(CombatMontageA, 1.5);
                AnimInstance->Montage_JumpToSection(FName("Attack_PrimaryA"), CombatMontageA);
                break;

            case 1:
                AnimInstance->Montage_Play(CombatMontageB, 2);
                AnimInstance->Montage_JumpToSection(FName("Attack_PrimaryB"), CombatMontageB);
                break;

            default: break;
            }
        }
    }
}

void AMainCharacter::AttackEnd() {
    SetBAttacking(false);
    SetInterpToEnemy(false);

    if (bLMBDown) {
        Attack();
    }
}

void AMainCharacter::EnableSwordCollision() {
    SwordHitbox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AMainCharacter::DisableSwordCollision() {
    SwordHitbox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AMainCharacter::DecrementHealth(float amount) {
    float HealthAmountToLose = GetCurrentHealth() - amount;

    if (GetCurrentHealth() - amount <= 0) {
        SetCurrentHealth(HealthAmountToLose);
        Die();
    }
    else {
        SetCurrentHealth(HealthAmountToLose);
    }

}

// TakeDamage() gathers more information than DecrementHealth() 
float AMainCharacter::TakeDamage(float DamageAmount,
                                 FDamageEvent const& DamageEvent,
                                 AController* EventInstigator,
                                 AActor* DamageCauser) {

    DecrementHealth(DamageAmount);

    return DamageAmount;
}


void AMainCharacter::Die() {

    if (GetCurrentHealth() <= 0) {
        UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
        if (AnimInstance && CombatMontageA && CombatMontageB) {
            AnimInstance->Montage_Play(CombatMontageA, 1.f);
            AnimInstance->Montage_JumpToSection(FName("Death"));
            GetCharacterMovement()->MaxWalkSpeed = 0;
            SetActorTickEnabled(false);
        }
    }
}

void AMainCharacter::DeathEnd() {
    GetMesh()->bPauseAnims = true;
    GetMesh()->bNoSkeletonUpdate = true;
}

void AMainCharacter::Vortex_OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                           UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                           const FHitResult& SweepResult) {

    /*
    *One way is to create a DamageOverTime component that you attach to an actor who is capable of receiving damage over time.
    The component should handle a "OnApplyDoT" event or have a function to explicitly apply a DoT whichever route you want.
    The Application of a DoT should provide the details, such as damage type, tick amount, tick frequency duration -- whatever makes sense for your game.
    Internally the component might keep track of many concurrent DoTs running at once. maybe it stacks them, or replaces them or refreshes a timer.
    Whatever details you wish to track will be variables of the component. The DameOverTimeComponent Tick event should be the one to apply the damage to it's owning pawn.
    You should definitely reduce the tick frequency of this component by setting it with SetComponentTickInterval in OnBeginPlay. https://docs.unrealengine.com/latest/INT/BlueprintAPI/Utilities/SetComponentTickInterval/index.html
    
    Then you put that component on whatever actors are capable of receiving DoT. Finally, when a damage source wants to dish out sweet justice, try to get the actor's DamageOverTimeComponent.
    If that succeeds, broadcast the event to it or call the function. You might also want to include the original actor who dealt the damage if you want to track who-done-it...
     *
     *
     */
    if (OtherActor) {

        AEnemyA* Enemy = Cast<AEnemyA>(OtherActor);
        if (Enemy) {
            // DS("Vortex - Overlap Begin");
            if (!bVortexDealDmg) {
                bVortexDealDmg = true;

                DamagedEnemyByVortex = Enemy;
            }
        }
    }
}

void AMainCharacter::Vortex_OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                         UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {

    if (OtherActor) {

        AEnemyA* Enemy = Cast<AEnemyA>(OtherActor);
        if (Enemy) {
            if (bVortexDealDmg) {
                bVortexDealDmg = false;
                DamagedEnemyByVortex = nullptr;
                // DS("Vortex - Overlap End");
            }
        }
    }
}

void AMainCharacter::RockBurst_OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
    if (OtherActor) {

        AEnemyA* Enemy = Cast<AEnemyA>(OtherActor);
        if (Enemy) {
            // DS("Rock Burst - Overlap Begin");
            if (!bRockBurstDealDmg) {
                bRockBurstDealDmg = true;

                DamagedEnemyByRockBurst = Enemy;
            }
        }
    }
}

void AMainCharacter::RockBurst_OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
    if (OtherActor) {

        AEnemyA* Enemy = Cast<AEnemyA>(OtherActor);
        if (Enemy) {
            if (bRockBurstDealDmg) {
                bRockBurstDealDmg = false;
                DamagedEnemyByRockBurst = nullptr;
                // DS("RockBurst - Overlap End");
            }
        }
    }
}

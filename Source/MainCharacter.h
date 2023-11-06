// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "UObject/ObjectMacros.h"

#include "MainCharacter.generated.h"

UENUM(BlueprintType)
enum class EManaRegenStatus :uint8 {
    EMRS_NORMAL UMETA(DisplayName = "Normal Regen"),
    EMRS_POTION UMETA(DisplayName = "Potion Regen"),

    EMRS_MAX UMETA(DisplayName = "DefaultMax")
};

UCLASS()
class PROJECTB_API AMainCharacter : public ACharacter {
    GENERATED_BODY()

public:
    // Sets default values for this character's properties
    AMainCharacter();

    // bool to detect if the Q Key is pressed (for the 1st ability, Vortex)
    bool bQKeyDown;
    // bool to detect if the W Key is pressed (for the 2st ability, Rock Burst)
    bool bWKeyDown;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat");
    bool bHasCombatTarget;
    FORCEINLINE void SetHasCombatTarget(bool HasTarget) { bHasCombatTarget = HasTarget; }
    UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Combat")
    FVector CombatTargetLocation;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Controller")
    class AMainPlayerController* MainPlayerController;

    // getter for the topdown follow camera
    FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownFollowCamera; }
    // getter for the camera boom
    FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
    // getter for the cursorToWorld component
    FORCEINLINE class UDecalComponent* GetCursorToWorld() { return CursorToWorld; }

    /* speed at which the player character should interpolate to the target
     * @param InterpSpeed takes in a float for the rotation
     */
    UPROPERTY()
    float InterpSpeed;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
    class UBoxComponent* SwordHitbox;

    // bool to keep track if the player should interpolate to the enemy or not
    UPROPERTY()
    bool bInterpToEnemy;

    // reference to the target the player should interpolate to 
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
    class AEnemyA* CombatTarget;

    // setter for the combat target
    FORCEINLINE void SetInterpCombatTarget(AEnemyA* Target) { CombatTarget = Target; }

    // DamageTypeClass
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    TSubclassOf<UDamageType> DamageTypeClass;

   
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities|Vortex")
    class UParticleSystemComponent* VortexParticleComponent;
    class UParticleSystem* VortexTemplate;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities|RockBurst")
    class UParticleSystemComponent* RockBurstParticleComponent;
    class UParticleSystem* RockBurstTemplate;

    //
    //////////////////////
    // Player Abilities //
    //////////////////////
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Abilities|Vortex")
    class USphereComponent* VortexAbility_Hitbox;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Abilities|Vortex")
    class USphereComponent* RockBurstAbility_Hitbox;


protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

private:
    // Top Down Follow Camera
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    class UCameraComponent* TopDownFollowCamera;

    // Camera Boom position the camera behind the player
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    class USpringArmComponent* CameraBoom;

    // Decal that projects to the cursor location
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    class UDecalComponent* CursorToWorld;

    //
    //////////////////
    // Player Stats //
    //////////////////
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerStats", meta = (AllowPrivateAccess = "true"))
    float MaxHealth;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerStats", meta = (AllowPrivateAccess = "true"))
    float Health;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerStats", meta = (AllowPrivateAccess = "true"))
    float MaxMana;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerStats", meta = (AllowPrivateAccess = "true"))
    float ManaRegen;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerStats", meta = (AllowPrivateAccess = "true"))
    float Mana;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerStats", meta = (AllowPrivateAccess = "true"))
    float WeaponDamage;


    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character", meta = (AllowPrivateAccess = "true"))
    float MovementSpeed;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character", meta = (AllowPrivateAccess = "true"))
    float ChannelingSpeed;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums", meta = (AllowPrivateAccess = "true"))
    EManaRegenStatus ManaStatus;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Potions", meta = (AllowPrivateAccess = "true"))
    float ManaRegenBuffDuration;

    //
    //////////////////
    // Vortex Stats //
    //////////////////
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities|Vortex", meta = (AllowPrivateAccess = "true"))
    FVector VortexAbility_InitialAoE;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities|Vortex", meta = (AllowPrivateAccess = "true"))
    float VortexAbility_AoE;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities|Vortex", meta = (AllowPrivateAccess = "true"))
    float VortexAbility_Damage;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities|Vortex", meta = (AllowPrivateAccess = "true"))
    float VortexAbility_ManaDrain;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities|Vortex", meta = (AllowPrivateAccess = "true"))
    float VortexAbility_InitialManaCost;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities|Vortex", meta = (AllowPrivateAccess = "true"))
    int32 VortexAbility_MaxStacks;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities|Vortex", meta = (AllowPrivateAccess = "true"))
    int32 VortexAbility_CurrentStacks;
    //
    //////////////////
    // Rock Burst Stats //
    //////////////////
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities|RockBurst", meta = (AllowPrivateAccess = "true"))
    float RockBurstAbility_AoE;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities|RockBurst", meta = (AllowPrivateAccess = "true"))
    float RockBurstAbility_Damage;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities|RockBurst", meta = (AllowPrivateAccess = "true"))
    float RockBurstAbility_InitialManaCost;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities|RockBurst", meta = (AllowPrivateAccess = "true"))
    float RockBurstAbility_Cooldown;


public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // Called to bind functionality to input
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    // get the lookatrotation.yaw for the target for interpolation
    FRotator GetLookAtRotationYaw(FVector Target);

    // functions to implement functionality when a key has been pressed (keyboard or mouse)
    bool bLMBDown;
    void LMBDown();
    void LMBUp();

    float fElapsedTime_ManaPotion;
    float fElapsedTime_RockBurstCooldown;
    float bCanCast_RockBurst;

    // press Q to cast ability 1
    void QKeyDown();
    // release func to cast ability 1
    void QKeyUp();

    // press W to cast ability 2
    void WKeyDown();
    // release func to cast ability 2
    void WKeyUp();


    // setter for the interp to enemy bool
    UFUNCTION()
    void SetInterpToEnemy(bool Interp);

    // reference to the currently equipped weapon
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Items")
    class AWeapon* EquippedWeapon;

    // reference for the currently overlapping item with the player
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Items")
    class AItem* ActiveOverlappingItem;

    // setter and getter for the weapon to equip
    void SetEquippedWeapon(AWeapon* WeaponToEquip);
    FORCEINLINE AWeapon* GetEquippedWeapon() { return EquippedWeapon; }

    // setter for the currently overlapping item
    FORCEINLINE void SetActiveOverlappingItem(AItem* ItemToEquip) { ActiveOverlappingItem = ItemToEquip; }

    // overlap to detect when the weapon hitbox overlaps with an enemy
    UFUNCTION()
    void Attack_OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                               UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                               const FHitResult& SweepResult);

    UFUNCTION()
    void Attack_OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                             UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


    // boolean to keep track whether the player is attacking or not
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anims")
    bool bAttacking;

    // getter and setter for bool IsAttacking
    FORCEINLINE void SetBAttacking(bool isAttacking) { bAttacking = isAttacking; }
    FORCEINLINE bool GetBAttacking() { return bAttacking; }

    /* the player attack function
     * checks if bAttacking = true
     * if its not it sets it to true and calls the SetInterpToEnemy function to true
     * then randomly picks one of 2 attack animations to play
     */
    void Attack();

    /* sets bAttacking = false
     * sets interpToEnemy to false
     * checks if the Left Mouse Button is being held down, if it is, call the Attack() function again
     */
    UFUNCTION(BlueprintCallable)
    void AttackEnd();

    // enable collision for the sword hitbox
    UFUNCTION(BlueprintCallable)
    void EnableSwordCollision();
    // disable collision for the sword hitbox
    UFUNCTION(BlueprintCallable)
    void DisableSwordCollision();


    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anims")
    class UAnimMontage* CombatMontageA;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anims")
    class UAnimMontage* CombatMontageB;


    /* decreases the player health
     * @param float amount is the amount by which you want to decrease the player health
     */
    void DecrementHealth(float amount);

    virtual float TakeDamage(float DamageAmount,
                             struct FDamageEvent const& DamageEvent,
                             class AController* EventInstigator,
                             AActor* DamageCauser) override;


    // DEATH!!!
    void Die();
    UFUNCTION(BlueprintCallable)
    void DeathEnd();
    bool bIsDead;

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////TESTING//////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn")
    TSubclassOf<class AEnemyA> PawnToSpawn;

    UFUNCTION(BlueprintCallable, Category = "Spawn")
    void SpawnCustomPawn(UClass* ToSpawn, const FVector& Location);

    UFUNCTION()
    void Vortex_OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                               UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                               const FHitResult& SweepResult);

    UFUNCTION()
    void Vortex_OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                             UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
    
    UFUNCTION()
    void RockBurst_OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                               UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                               const FHitResult& SweepResult);

    UFUNCTION()
    void RockBurst_OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                             UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


    //////////////////////////////////////
    ///Player Stats Getters and Setters///
    //////////////////////////////////////new

    FORCEINLINE float GetMaxHealth() const { return MaxHealth; }
    FORCEINLINE void SetMaxHealth(float newMaxHealth) { MaxHealth = newMaxHealth; }

    FORCEINLINE float GetCurrentHealth() const { return Health; }
    FORCEINLINE void SetCurrentHealth(float newHealth) { Health = newHealth; }

    FORCEINLINE float GetMaxMana() const { return MaxMana; }
    FORCEINLINE void SetMaxMana(float newMaxMana) { MaxMana = newMaxMana; }

    FORCEINLINE float GetManaRegen() const { return ManaRegen; }
    FORCEINLINE void SetManaRegen(float newManaRegen) { ManaRegen = newManaRegen; }

    FORCEINLINE float GetCurrentMana() const { return Mana; }
    FORCEINLINE void SetCurrentMana(float newMana) { Mana = newMana; }

    FORCEINLINE float GetCurrentWeaponDmg() const { return WeaponDamage; }
    FORCEINLINE void SetCurrentWeaponDmg(float newWeaponDmg) { WeaponDamage = newWeaponDmg; }

    FORCEINLINE float GetPlayerMovementSpeed() const { return MovementSpeed; }
    FORCEINLINE void SetPlayerMovementSpeed(float moveSpeed) { MovementSpeed = moveSpeed; }

    FORCEINLINE float GetPlayerMovement_ChannelingSpeed() const { return ChannelingSpeed; }
    FORCEINLINE void SetPlayerMovement_ChannelingSpeed(float channelingSpeed) { ChannelingSpeed = channelingSpeed; }

    FORCEINLINE EManaRegenStatus GetManaStatus() const { return ManaStatus; }
    FORCEINLINE void SetManaStatus(EManaRegenStatus newStatus) { ManaStatus = newStatus; }
    
    FORCEINLINE float GetManaRegenBuffDuration() const { return ManaRegenBuffDuration; }
    FORCEINLINE void SetManaRegenBuffDuration(float buffDuration) { ManaRegenBuffDuration = buffDuration; }


    ////////////////////////////////
    ///Vortex Getters and Setters///
    ////////////////////////////////
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities|Vortex")
    bool bVortexDealDmg = false;

    AEnemyA* DamagedEnemyByVortex;
    FTimerHandle DmgTimerHandle;
    float DmgInterval = 1.f;

    FORCEINLINE FVector GetVortexAbility_InitialAoE() const { return VortexAbility_InitialAoE; }
    FORCEINLINE void SetVortexAbility_InitialAoE(FVector InitialAoE) {
        VortexAbility_InitialAoE = InitialAoE;
    }

    FORCEINLINE float GetVortexAbility_CurrentAoE() const { return VortexAbility_AoE; }
    FORCEINLINE void SetVortexAbility_CurrentAoE(float AoE) { this->VortexAbility_AoE = AoE; }

    FORCEINLINE float GetVortexAbility_Damage() const { return VortexAbility_Damage; }
    FORCEINLINE void SetVortexAbility_Damage(float Damage) {
        this->VortexAbility_Damage = Damage;
    }

    FORCEINLINE float GetVortexAbility_InitialManaCost() const { return VortexAbility_InitialManaCost; }
    FORCEINLINE void SetVortexAbility_InitialManaCost(float InitialManaCost) {
        this->VortexAbility_InitialManaCost = InitialManaCost;
    }

    FORCEINLINE float GetVortexAbility_ManaDrain() const { return VortexAbility_ManaDrain; }
    FORCEINLINE void SetVortexAbility_ManaDrain(float ManaDrain) {
        this->VortexAbility_ManaDrain = ManaDrain;
    }

    FORCEINLINE int32 GetVortexAbility_MaxStacks() const { return VortexAbility_MaxStacks; }
    FORCEINLINE void SetVortexAbility_MaxStacks(int32 MaxStacks) {
        this->VortexAbility_MaxStacks = MaxStacks;
    }

    FORCEINLINE int32 GetVortexAbility_CurrentStacks() const { return VortexAbility_CurrentStacks; }
    FORCEINLINE void SetVortexAbility_CurrentStacks(int32 CurrentStacks) {
        this->VortexAbility_CurrentStacks = CurrentStacks;
    }
    ////////////////////////////////
    ///RockBurst Getters and Setters///
    ////////////////////////////////
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities|RockBurst")
    bool bRockBurstDealDmg = false;

    AEnemyA* DamagedEnemyByRockBurst;

    FORCEINLINE float GetRockBurstAbility_CurrentAoE() const { return RockBurstAbility_AoE; }
    FORCEINLINE void SetRockBurstAbility_CurrentAoE(float AoE) { this->RockBurstAbility_AoE = AoE; }

    FORCEINLINE float GetRockBurstAbility_Damage() const { return RockBurstAbility_Damage; }
    FORCEINLINE void SetRockBurstAbility_Damage(float Damage) {
        this->RockBurstAbility_Damage = Damage;
    }

    FORCEINLINE float GetRockBurstAbility_InitialManaCost() const { return RockBurstAbility_InitialManaCost; }
    FORCEINLINE void SetRockBurstAbility_InitialManaCost(float InitialManaCost) {
        this->RockBurstAbility_InitialManaCost = InitialManaCost;
    }

    FORCEINLINE float GetRockBurstAbility_Cooldown() const { return RockBurstAbility_Cooldown; }
    FORCEINLINE void SetRockBurstAbility_Cooldown(float CooldownAmount) {
        this->RockBurstAbility_Cooldown = CooldownAmount;
    }

};


// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Character.h"
#include "EnemyA.generated.h"


// enum that contains the different states the enemy can be in
UENUM(BlueprintType)
enum class EEnemyMovementStatus :uint8 {
	EMS_IDLE			UMETA(DisplayName = "Idle"),
    EMS_MoveToTarget	UMETA(DisplayName = "MoveToTarget"),
    EMS_ATTACKING		UMETA(DisplayName = "Attacking"),
	EMS_DEAD		UMETA(DisplayName = "Dead"),

    EMS_MAX				UMETA(DisplayName = "DefaultMax"),
};

UCLASS()
class PROJECTB_API AEnemyA : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyA();

	// init of enemy movement status enum
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	EEnemyMovementStatus EnemyMovementStatus;

	// setter for the enemy state
	FORCEINLINE void SetEnemyMovementStatus(EEnemyMovementStatus status) { EnemyMovementStatus = status; }
	FORCEINLINE EEnemyMovementStatus GetEnemyMovementStatus() { return EnemyMovementStatus; }

	/* speed at which the enemy character should interpolate to the target
	* @param InterpSpeed takes in a float for the rotation
	*/
	UPROPERTY()
	float InterpSpeed;

	// bool to keep track if the enemy should interpolate to the target or not
	UPROPERTY()
	bool bInterpToTarget;

	

	// aggro sphere is the radius in which when the player enters the enemy "detects" the player and starts moving towards him
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class USphereComponent* AggroSphere;

	/* combat sphere is the radius in which when the player enters the enemy changes to the attacking state and calls
	 corresponding functions */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	USphereComponent* CombatSphere;
	
// init of the enemy attack hitbox. It is bound to the mesh claw and is used to collision detection with the player
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	class UBoxComponent* AttackHitbox;

	
	// reference to the enemy AIController
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class AAIController* AIController;

	// reference to the enemy combat montage
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class UAnimMontage* CombatMontage;

	// particle to play when enemy is hit
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	class UParticleSystem* HitParticle;

	UPROPERTY()
	FTimerHandle AttackTimer;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackMinTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackMaxTime;

	
	// Enemy stat variables
	// max enemy health
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float MaxHealth;

	// current enemy health
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float Health;

	// enemy damage per normal attack
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TSubclassOf<UDamageType> DamageTypeClass;

	// reference to the target the enemy should interpolate to
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	class AMainCharacter* InterpTarget;
	// setter for the target to interpolate to
	FORCEINLINE void SetInterpTarget(AMainCharacter* Target) { InterpTarget = Target; }

	FTimerHandle DeathTimer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float CorpseLifespan;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// setter for the interp to target player bool
	UFUNCTION()
	void SetInterpToPlayerTarget(bool Interp);

	FRotator GetLookAtRotationYaw(FVector Target);

	// called when the player enters the aggro range of the enemy and calls the MoveTo functionality
	UFUNCTION()
	virtual void AggroSphere_OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// called when the player leaves the aggro range of the enemy
	UFUNCTION()
    virtual void AggroSphere_OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// called when the player enters the combat range of the enemy and calls the Attack funcionality
	UFUNCTION()
    virtual void CombatSphere_OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// called when the player leaves the aggro range of the enemy
	UFUNCTION()
    virtual void CombatSphere_OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// called when the player hitbox collides with the enemy attack hitbox. used to deal damage to the player
	UFUNCTION()
	void AttackHOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// called when the player hitbox collides with the enemy attack hitbox
	UFUNCTION()
	void AttackHOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// function that sets the enemy to move to the given target
	UFUNCTION(BlueprintCallable)
	void MoveToTarget(class AMainCharacter* Target);

	
	// boolean to detect when the player is in the enemy combat sphere
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI");
	bool bOverlappingCombatSphere;

	// reference to the 
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
	AMainCharacter* CombatTarget;

	
	// enables collision for the enemy attack hitbox
	UFUNCTION(BlueprintCallable)
	void EnableCollision();

	// disables collision for the enemy attack hitbox
	UFUNCTION(BlueprintCallable)
	void DeactivateCollision();


	// bool to keep track if the enemy is currently attacking
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bAttacking;

	// sets enemy to attacking state, stops movement and plays the attack animation
	void Attack();

	// sets bAttacking to false which stops the attacking state
	UFUNCTION(BlueprintCallable)
	void AttackEnd();

	virtual float TakeDamage(float DamageAmount,
                            struct FDamageEvent const& DamageEvent,
                             class AController* EventInstigator,
                             AActor* DamageCauser) override;

	void DecrementHealth(float DmgAmount);

	void Die();

	UFUNCTION(BlueprintCallable)
	void DeathEndAnim();

	bool Alive();

	void CorpseDisappear();
};

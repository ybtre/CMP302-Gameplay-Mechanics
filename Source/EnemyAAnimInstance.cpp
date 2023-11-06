// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAAnimInstance.h"
#include "EnemyA.h"

void UEnemyAAnimInstance::NativeInitializeAnimation() {
    if (Pawn == nullptr) {
        
        Pawn = TryGetPawnOwner();
        if (Pawn) {
            EnemyA = Cast<AEnemyA>(Pawn);
        }
    }
}

void UEnemyAAnimInstance::UpdateAnimationProperties() {
    if (Pawn == nullptr) {

        Pawn = TryGetPawnOwner();
        if (Pawn) {           
            EnemyA = Cast<AEnemyA>(Pawn);
        }
    }
    
    if (Pawn) {
        FVector Speed = Pawn->GetVelocity();
        FVector LateralSpeed = FVector(Speed.X, Speed.Y, 0);
        MovementSpeed = LateralSpeed.Size();
    }
}

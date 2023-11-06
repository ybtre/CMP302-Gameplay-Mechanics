// Fill out your copyright notice in the Description page of Project Settings.


#include "ManaRegenBuff.h"
#include "MainCharacter.h"

AManaRegenBuff::AManaRegenBuff() {
}

void AManaRegenBuff::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
    Super::OnOverlapBegin(
       OverlappedComponent,
       OtherActor,
       OtherComp,
       OtherBodyIndex,
       bFromSweep,
       SweepResult);

    UE_LOG(LogTemp, Warning, TEXT("ManaRegenBUFF::OnOverlapBegin"));

    if (OtherActor) {

        AMainCharacter* MainChar = Cast<AMainCharacter>(OtherActor);
        if (MainChar) {
            MainChar->SetManaStatus(EManaRegenStatus::EMRS_POTION);
            Destroy();
        }
    }
}

void AManaRegenBuff::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
    Super::OnOverlapEnd(
        OverlappedComponent,
        OtherActor,
        OtherComp,
        OtherBodyIndex);
}

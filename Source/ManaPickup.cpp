// Fill out your copyright notice in the Description page of Project Settings.


#include "ManaPickup.h"
#include "MainCharacter.h"

AManaPickup::AManaPickup() {
    AmountOfManaToRecover = 40.f;
}

void AManaPickup::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
    Super::OnOverlapBegin(
       OverlappedComponent,
       OtherActor,
       OtherComp,
       OtherBodyIndex,
       bFromSweep,
       SweepResult);

    UE_LOG(LogTemp, Warning, TEXT("ManaPotionPickup::OnOverlapBegin"));

    if (OtherActor) {

        AMainCharacter* MainChar = Cast<AMainCharacter>(OtherActor);
        if (MainChar) {
            auto CurrentMana = MainChar->GetCurrentMana();
            MainChar->SetCurrentMana(CurrentMana + AmountOfManaToRecover);
            Destroy();
        }
    }
}

void AManaPickup::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {

    Super::OnOverlapEnd(
        OverlappedComponent,
        OtherActor,
        OtherComp,
        OtherBodyIndex);
    
    UE_LOG(LogTemp, Warning, TEXT("ManaPotionPickup::OnOverLapEnd"));
}

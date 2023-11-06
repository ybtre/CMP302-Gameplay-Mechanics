// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickup.h"
#include "MainCharacter.h"

APickup::APickup() {

    PotionHealAmount = 30.f;
}

void APickup::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {

    // Using Super + function in order to ensure that the child functions get called
    // because this is a virtual override function of the Item Class
    Super::OnOverlapBegin(
        OverlappedComponent,
        OtherActor,
        OtherComp,
        OtherBodyIndex,
        bFromSweep,
        SweepResult);

    UE_LOG(LogTemp, Warning, TEXT("HealPotionPickup::OnOverlapBegin"));

    if (OtherActor) {

        AMainCharacter* MainChar = Cast<AMainCharacter>(OtherActor);
        if (MainChar) {
            auto CurrentHealth = MainChar->GetCurrentHealth();
            MainChar->SetCurrentHealth(CurrentHealth + PotionHealAmount);
            Destroy();
        }
    }
}

void APickup::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex) {

    // Using Super + function in order to ensure that the child functions get called
    // because this is a virtual override function of the Item Class
    Super::OnOverlapEnd(
        OverlappedComponent,
        OtherActor,
        OtherComp,
        OtherBodyIndex);

    UE_LOG(LogTemp, Warning, TEXT("HealPotionPickup::OnOverLapEnd"));
}

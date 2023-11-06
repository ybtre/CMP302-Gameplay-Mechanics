// Fill out your copyright notice in the Description page of Project Settings.


#include "Explosive.h"
#include "MainCharacter.h"

AExplosive::AExplosive() {

    Damage = 20.f;
}

void AExplosive::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {

    // Using Super + function in order to ensure that the child functions get called
    // because this is a virtual override function of the Item Class
    Super::OnOverlapBegin(
        OverlappedComponent, 
        OtherActor,OtherComp,
        OtherBodyIndex,
        bFromSweep,
        SweepResult);

    UE_LOG(LogTemp, Warning, TEXT("Explosive::On Overlap Begin"));

    if (OtherActor) {

        AMainCharacter* MainChar = Cast<AMainCharacter>(OtherActor);
        if (MainChar) {

            MainChar->DecrementHealth(Damage);
            Destroy();
        }
    }
}

void AExplosive::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {

    // Using Super + function in order to ensure that the child functions get called
    // because this is a virtual override function of the Item Class
    Super::OnOverlapEnd(
        OverlappedComponent,
        OtherActor,
        OtherComp,
        OtherBodyIndex);

    UE_LOG(LogTemp, Warning, TEXT("Explosive::On Overlap Begin"));
}

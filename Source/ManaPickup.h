// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "ManaPickup.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTB_API AManaPickup : public AItem
{
	GENERATED_BODY()

public:

	AManaPickup();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Potions")
	float AmountOfManaToRecover;

	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;
};

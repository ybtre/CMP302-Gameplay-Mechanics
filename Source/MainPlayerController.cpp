// Fill out your copyright notice in the Description page of Project Settings.


#include "MainPlayerController.h"


#include "MainCharacter.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Blueprint/UserWidget.h"
#include "Components/DecalComponent.h"
#include "Macros.h"

AMainPlayerController::AMainPlayerController() {
    bShowMouseCursor = true;
    DefaultMouseCursor = EMouseCursor::Crosshairs;
}

void AMainPlayerController::BeginPlay() {
	Super::BeginPlay();

    if(HUDOverlayAsset) {
        HUDOverlay = CreateWidget<UUserWidget>(this, HUDOverlayAsset);
    }

    HUDOverlay->AddToViewport();
    HUDOverlay->SetVisibility(ESlateVisibility::Visible);

    if(WEnemyHealthBar) {
        EnemyHealthBar = CreateWidget<UUserWidget>(this, WEnemyHealthBar);
        if(EnemyHealthBar) {
            EnemyHealthBar->AddToViewport();
            EnemyHealthBar->SetVisibility(ESlateVisibility::Hidden);
        }
        FVector2D Alignment(0, 0);
        EnemyHealthBar->SetAlignmentInViewport(Alignment);
    }
}

void AMainPlayerController::Tick(float DeltaSeconds) {
    Super::Tick(DeltaSeconds);

    if (EnemyHealthBar) {
        FVector2D PositionInViewport;
        ProjectWorldLocationToScreen(EnemtLocation, PositionInViewport);
        PositionInViewport.X -= 70.f;
        PositionInViewport.Y -= 100.f;

        FVector2D SizeInViewport(200.f, 15.f);

        EnemyHealthBar->SetPositionInViewport(PositionInViewport);
        EnemyHealthBar->SetDesiredSizeInViewport(SizeInViewport);
    }
}

void AMainPlayerController::PlayerTick(float DeltaTime) {
    Super::PlayerTick(DeltaTime);

    // keep updating the destination every tick while desired
    if (bMoveToMouseCursor) {
        // if (AMainPlayerController* PlayerController = Cast<AMainPlayerController>(GetController()))
        MoveToMouseCursor();
    }
}

void AMainPlayerController::SetupInputComponent() {
    Super::SetupInputComponent();

    InputComponent->BindAction("RMB", IE_Pressed, this, &AMainPlayerController::OnSetDestinationPressed);
    InputComponent->BindAction("RMB", IE_Released, this, &AMainPlayerController::OnSetDestinationReleased);
}

void AMainPlayerController::MoveToMouseCursor() {
        FHitResult Hit;
        GetHitResultUnderCursor(ECC_Visibility, false, Hit);
    
        if (Hit.bBlockingHit) {
            // we hit something, move there
            SetNewMoveDestination(Hit.ImpactPoint);
        }
}

void AMainPlayerController::SetNewMoveDestination(const FVector DestLocation) {
    APawn* MainCharacterPawn = GetPawn();
    if (MainCharacterPawn) {
        float const Distance = FVector::Dist(DestLocation, MainCharacterPawn->GetActorLocation());

        // we need to issue move command only if far enough in order for walk animation to play correctly
        if(Distance > 5.f) {
            UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, DestLocation);
        }
    }
}

void AMainPlayerController::OnSetDestinationPressed() {
    // set flag to keep updating destination until released
    bMoveToMouseCursor = true;
}

void AMainPlayerController::OnSetDestinationReleased() {
    // clear flag to indicate we should stop updating the destination
    bMoveToMouseCursor = false;
}

void AMainPlayerController::DisplayEnemyHealthBar() {
    if (EnemyHealthBar) {
        bEnemyHealthBarVisible = true;
        EnemyHealthBar->SetVisibility(ESlateVisibility::Visible);
    }
}

void AMainPlayerController::RemoveEnemyHealthBar() {
    if (EnemyHealthBar) {
        bEnemyHealthBarVisible = false;
        EnemyHealthBar->SetVisibility(ESlateVisibility::Hidden);
    }
}


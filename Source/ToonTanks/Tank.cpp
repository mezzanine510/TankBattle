// Fill out your copyright notice in the Description page of Project Settings.


#include "Tank.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Kismet/GameplayStatics.h"


ATank::ATank()
{
    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
    SpringArm->SetupAttachment(RootComponent);

    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(SpringArm);
}

void ATank::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
    {
        EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATank::Move);
        EnhancedInputComponent->BindAction(TurnAction, ETriggerEvent::Triggered, this, &ATank::Turn);
    }
}

// Called every frame
void ATank::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (PlayerControllerRef)
    {
        FHitResult HitResult;

        PlayerControllerRef->GetHitResultUnderCursor(
            ECollisionChannel::ECC_Visibility,
            false,
            HitResult
        );

        RotateTurret(HitResult.ImpactPoint);
    }

}

void ATank::BeginPlay()
{
    Super::BeginPlay();

    PlayerControllerRef = Cast<APlayerController>(GetController());

    if (PlayerControllerRef)
    {
        if (UEnhancedInputLocalPlayerSubsystem* InputSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerControllerRef->GetLocalPlayer()))
        {
            InputSystem->AddMappingContext(PlayerMappingContext, 0);
        }
    }
}

void ATank::Move(const FInputActionValue& Value)
{
    const float CurrentValue = Value.Get<float>();
    FVector Location = FVector::ZeroVector;
    Location.X = CurrentValue * Speed * UGameplayStatics::GetWorldDeltaSeconds(this);
    AddActorLocalOffset(Location, true);
}

void ATank::Turn(const FInputActionValue& Value)
{
    const float CurrentValue = Value.Get<float>();
    FRotator Rotation = FRotator::ZeroRotator;
    Rotation.Yaw = CurrentValue * TurnRate * UGameplayStatics::GetWorldDeltaSeconds(this);
    AddActorLocalRotation(Rotation, true);
}


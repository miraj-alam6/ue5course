// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawns/Bird.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Components/InputComponent.h"



ABird::ABird()
{
	PrimaryActorTick.bCanEverTick = true;
	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	//RootComponent = Capsule; //No problem with doing this, but using the function anyways
	SetRootComponent(Capsule);

	Capsule->SetCapsuleHalfHeight(40.f);
	Capsule->SetCapsuleRadius(30.f);

	BirdMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BirdMesh"));
	//Capsule RootComponent and GetRootComponent() all do the same thing atm, I just use GetRootComponent()
	BirdMesh->SetupAttachment(GetRootComponent());

	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

void ABird::BeginPlay()
{
	Super::BeginPlay();
	
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController) {
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
		if (Subsystem) {
			Subsystem->AddMappingContext(BirdMappingContext, 0);
		}
	}
}

void ABird::OnPossess()
{
	//Add Input Mapping Context
	// This is how the enhanced input template project did the adding of the context, I do it a little differently
	//if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	//{
	//	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
	//	{
	//		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	//	}
	//}

	//I tried doing this OnPossess rather than just in BeginPlay like how he did, and things did not work. I'm not sure why.
	//APlayerController* PlayerController = Cast<APlayerController>(GetController());
	//if (PlayerController) {
	//	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
	//	if (Subsystem) {
	//		Subsystem->AddMappingContext(BirdMappingContext, 0);
	//	}
	//}
}

void ABird::MoveForwardCB(float Value)
{
	UE_LOG(LogTemp, Warning, TEXT("Axis value: %f"), Value);
}

void ABird::Move(const FInputActionValue& Value)
{
	const FVector2D CurrentValue = Value.Get<FVector2D>();
	UE_LOG(LogTemp, Warning, TEXT("X and Y values from new input system IA_Move %f %f"), CurrentValue.X, CurrentValue.Y);
}

void ABird::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABird::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABird::Move);
	}
	PlayerInputComponent->BindAxis(FName("MoveForward"), this, &ABird::MoveForwardCB);
}


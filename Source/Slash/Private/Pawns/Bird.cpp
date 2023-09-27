// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawns/Bird.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
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


	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 300.f;

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(CameraBoom);
	

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
	if (GetController() && (Value != 0.f)) {
		FVector Forward = GetActorForwardVector();
		AddMovementInput(Forward, Value);
	}
	UE_LOG(LogTemp, Warning, TEXT("Axis value: %f"), Value);
}

void ABird::Move(const FInputActionValue& Value)
{
	const FVector2D CurrentValue = Value.Get<FVector2D>();
	UE_LOG(LogTemp, Warning, TEXT("X and Y values from new input system IA_Move %f %f"), CurrentValue.X, CurrentValue.Y);
	float YValue = CurrentValue.Y;
	//This is what the dude did albeit with a float rather than vector
	//if (GetController() && (YValue != 0.f)) {
	//	FVector Forward = GetActorForwardVector();
	//	AddMovementInput(Forward, YValue);
	//}

	//Since i have a vector, I do things get movement in both axes
	//interesingly enough I had to flip Y and X to get the intended movement.
	//This is because our pawn is facing the direction of the X axis which is forward, but in terms of input Y axis is what represents going forward (W in WSDA is Y axis) 
	if (GetController() && (CurrentValue.SquaredLength() > 0.001)) {
		FVector Vector = FVector(CurrentValue.Y, CurrentValue.X, 0);
		AddMovementInput(Vector, 1.f);
	}
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
		//EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABird::Move);
	}

	PlayerInputComponent->BindAxis(FName("MoveForward"), this, &ABird::MoveForwardCB);
}


// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SlashCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GroomComponent.h"

#include "InputAction.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Components/InputComponent.h"



// Sets default values
ASlashCharacter::ASlashCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	//didn't just name it CharacterMovement because it hides an inherited member variable causing compilation to fail
	UCharacterMovementComponent* CharacterMovementPtr = GetCharacterMovement();
	CharacterMovementPtr->bOrientRotationToMovement = true;
	CharacterMovementPtr->RotationRate = FRotator(0.0, 720.0, 0.0);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 300.f;

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(CameraBoom);

	Hair = CreateDefaultSubobject<UGroomComponent>(TEXT("Hair"));
	Hair->SetupAttachment(GetMesh());
	Hair->AttachmentName = FString("head");

	Eyebrows = CreateDefaultSubobject<UGroomComponent>(TEXT("Eyebrows"));
	Eyebrows->SetupAttachment(GetMesh());
	Eyebrows->AttachmentName = FString("head");

}

// Called when the game starts or when spawned
void ASlashCharacter::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController) {
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
		if (Subsystem) {
			Subsystem->AddMappingContext(SlashContext, 0);
		}
	}
	
}

// Called every frame
void ASlashCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASlashCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		EnhancedInputComponent->BindAction(MovementAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Look);
		//This is just a reminder that Released Does work even for things that are axes in value type. It seems it completes once it reaches zero aka you're not pressing any of the inputs for the mapping.
		//EnhancedInputComponent->BindAction(MovementAction, ETriggerEvent::Completed, this, &ACharacter::Jump);

		// For Jump bind to the built in jump function for Character Movement, we don't have to make our own function
		//I also bind to released in order to utilize Jump Max Hold Time in Character, other the jump will behave like you're holding the full time even if you tap the button quickly
		//IMPORTANT: I DID NOT USE TRIGGERED AND INSTEAD USED STARTED in order to prevent bunny hopping from holding the jump button
		//EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

	}

	//Fully functional non enhanced input
	//PlayerInputComponent->BindAxis(FName("MoveForward"), this, &ASlashCharacter::MoveForwardCB);
	//PlayerInputComponent->BindAxis(FName("Turn"), this, &ASlashCharacter::TurnCB);
	//PlayerInputComponent->BindAxis(FName("LookUp"), this, &ASlashCharacter::LookUpCB);
	//PlayerInputComponent->BindAxis(FName("MoveRight"), this, &ASlashCharacter::MoveRightCB);

	//Jump with non enhanced input, bind to the built in jump function. we don't have to make our own function
	//I also bind to released in order to utilize Jump Max Hold Time in Character
	//without calling StopJumping, the jump will behave as if you're holding the whole time
	//PlayerInputComponent->BindAction(FName("Jump"), EInputEvent::IE_Pressed, this, &ACharacter::Jump);
	//PlayerInputComponent->BindAction(FName("Jump"), EInputEvent::IE_Released, this, &ACharacter::StopJumping);
}

void ASlashCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D MovementVector =  Value.Get<FVector2D>();
	
	//This first implementation Only works for fixed camera
	//const FVector Forward = GetActorForwardVector();
	//AddMovementInput(Forward, MovementVector.Y);
	//const FVector Right = GetActorRightVector();
	//AddMovementInput(Right, MovementVector.X);

	//This works with rotatable camera (controller's rotation)
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	AddMovementInput(ForwardDirection, MovementVector.Y);

	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	AddMovementInput(RightDirection, MovementVector.X);
}

void ASlashCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);
}

void ASlashCharacter::MoveForwardCB(float Value)
{
	if (GetController() && (Value != 0.f)) {
		//Doesn't work with camera's rotation
		//FVector Forward = GetActorForwardVector();
		//AddMovementInput(Forward, Value);

		//Correct way to find out which way is forward
		const FRotator ControlRotation = GetControlRotation();
		const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}


}

void ASlashCharacter::MoveRightCB(float Value)
{
	if (GetController() && (Value != 0.f)) {
		//Doesn't work with camera's rotation
		//FVector Right = GetActorRightVector();
		//AddMovementInput(Right, Value);

		//Correct way to find out which way is right
		const FRotator ControlRotation = GetControlRotation();
		const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}


}

void ASlashCharacter::TurnCB(float Value)
{
	AddControllerYawInput(Value);
}

void ASlashCharacter::LookUpCB(float Value)
{
	AddControllerPitchInput(Value);
}

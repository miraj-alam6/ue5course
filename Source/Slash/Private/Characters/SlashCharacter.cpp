// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SlashCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"


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

}

// Called when the game starts or when spawned
void ASlashCharacter::BeginPlay()
{
	Super::BeginPlay();
	
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

	PlayerInputComponent->BindAxis(FName("MoveForward"), this, &ASlashCharacter::MoveForwardCB);
	PlayerInputComponent->BindAxis(FName("Turn"), this, &ASlashCharacter::TurnCB);
	PlayerInputComponent->BindAxis(FName("LookUp"), this, &ASlashCharacter::LookUpCB);
	PlayerInputComponent->BindAxis(FName("MoveRight"), this, &ASlashCharacter::MoveRightCB);
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

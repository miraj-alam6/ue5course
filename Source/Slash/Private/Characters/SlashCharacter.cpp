// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SlashCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/BoxComponent.h"
#include "GroomComponent.h"
#include "Items/Item.h"
#include "Items/Weapons/Weapon.h"
#include "Camera/PlayerCameraManager.h"


#include "InputAction.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Components/InputComponent.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimInstance.h"



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
		EnhancedInputComponent->BindAction(EKeyAction, ETriggerEvent::Started, this, &ASlashCharacter::EKeyPressedCB);
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
	//PlayerInputComponent->BindAction(FName("Equip"), EInputEvent::IE_Pressed, this, &ASlashCharacter::EKeyPressed);
	PlayerInputComponent->BindAction(FName("Attack"), EInputEvent::IE_Pressed, this, &ASlashCharacter::AttackCB);
}

void ASlashCharacter::SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled)
{
	if (EquippedWeapon && EquippedWeapon->GetWeaponBox()) {
		if (CollisionEnabled == ECollisionEnabled::NoCollision) {
			EquippedWeapon->IgnoreActors.Empty();
		}
		else {
			//From the weapon's point of view it should ignore itself.
			EquippedWeapon->IgnoreActors.AddUnique(EquippedWeapon);
		}
		EquippedWeapon->GetWeaponBox()->SetCollisionEnabled(CollisionEnabled);
	}
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
	//Only move if unoccupied, no moving and attacking in this game.
	//Dude in video did it a bit differently, simply returning if we are already attacking


	if (ActionState == EActionState::EAS_Unoccupied) {
		const FRotator Rotation = Controller->GetControlRotation();

		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(ForwardDirection, MovementVector.Y);

		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}

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

void ASlashCharacter::EKeyPressedCB()
{
	AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem);
	if (OverlappingWeapon) {
		OverlappingWeapon->Equip(GetMesh(), FName(TEXT("RightHandSocket")), this, this);
		CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
		EquippedWeapon = OverlappingWeapon;
		OverlappingItem = nullptr;
	}

	else {
		if (CanDisarm()) {
			//This unequip is different from having a weapon equipped, just means if it's on your hand vs back
			PlayEquipMontage(FName("Unequip"));
			CharacterState = ECharacterState::ECS_Unequipped;
			ActionState = EActionState::EAS_EquippingWeapon;
		}
		else if (CanArm()) {
			//This equip is different from having a weapon equipped, just means if it's on your hand vs back
			PlayEquipMontage(FName("Equip"));
			CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
			ActionState = EActionState::EAS_EquippingWeapon;
		}
	}
}

void ASlashCharacter::AttackCB()
{
	//just made a function instead of doing local variable
	//const bool bCanAttack = ActionState == EActionState::EAS_Unoccupied 
	//	&& CharacterState != ECharacterState::ECS_Unequipped;
	//if (bCanAttack) {
	if (CanAttack()) {
		PlayAttackMontage();
		ActionState = EActionState::EAS_Attacking;
	}
}

bool ASlashCharacter::CanAttack()
{
	return ActionState == EActionState::EAS_Unoccupied
	&& CharacterState != ECharacterState::ECS_Unequipped;
}

bool ASlashCharacter::CanDisarm()
{
	return ActionState == EActionState::EAS_Unoccupied
		&& CharacterState != ECharacterState::ECS_Unequipped;
}

bool ASlashCharacter::CanArm()
{
	return ActionState == EActionState::EAS_Unoccupied
		&& CharacterState == ECharacterState::ECS_Unequipped
		&& EquippedWeapon;
}

void ASlashCharacter::Disarm()
{
	if (EquippedWeapon) {
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("SpineSocket"));
	}
}

void ASlashCharacter::Arm()
{
	if (EquippedWeapon) {
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("RightHandSocket"));
	}
}

void ASlashCharacter::FinishEquipping()
{
	ActionState = EActionState::EAS_Unoccupied;
}


void ASlashCharacter::PlayAttackMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && AttackMontage) {
		AnimInstance->Montage_Play(AttackMontage);
		//Uncomment out to get random attack
		const int32 Selection = FMath::RandRange(0, 3);
		//Uncomment out to get a single attack always
		//const int32 Selection = 1;
		FName SectionName = FName();
		switch (Selection) {
		case 0:
			SectionName = FName("Attack1");
			break;
		case 1:
			SectionName = FName("Attack2");
			break;
		case 2:
			SectionName = FName("Attack3");
			break;
		case 3:
			SectionName = FName("Attack4");
			break;
		default:
			SectionName = FName("Attack1");
		}
		AnimInstance->Montage_JumpToSection(SectionName, AttackMontage);
	}
}

void ASlashCharacter::PlayEquipMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && EquipMontage) {
		AnimInstance->Montage_Play(EquipMontage);
		AnimInstance->Montage_JumpToSection(SectionName, EquipMontage);
	}
}

void ASlashCharacter::AttackEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

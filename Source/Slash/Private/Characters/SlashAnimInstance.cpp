// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SlashAnimInstance.h"

#include "Characters/SlashCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void USlashAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	//SlashCharacter as well many other variables in this function are member
	//variable pointers that are nullptr by default and we set them by just getting stuff
	//from the pawn owner
	SlashCharacter = Cast<ASlashCharacter>(TryGetPawnOwner());
	if (SlashCharacter) {
		SlashCharacterMovement = SlashCharacter->GetCharacterMovement();
	}
}

void USlashAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (SlashCharacterMovement) {
		GroundSpeed = UKismetMathLibrary::VSizeXY(SlashCharacterMovement->Velocity);	
		bIsFalling = SlashCharacterMovement->IsFalling();
	}
}

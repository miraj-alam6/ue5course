// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
//He included this here because you can't forward declare structs, but he misspoke. In our case we actually can forward declare the struct because it's a reference to the struct(signature of void Move function), so we don't need the size of it
//#include "InputActionValue.h"


#include "Bird.generated.h"


//Forward declarations
class UCapsuleComponent;
class USkeletalMeshComponent;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;

//only can do this forward declaration because the header has a reference to a struct rather than passing it by value
struct FInputActionValue;

UCLASS()
class SLASH_API ABird : public APawn
{
	GENERATED_BODY()

public:
	ABird();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess();
	void MoveForwardCB(float Value);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputMappingContext* BirdMappingContext;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* MoveAction;


	void Move(const FInputActionValue& Value);
private:
	UPROPERTY(VisibleAnywhere)
	//class UCapsuleComponent* Capsule; //This a valid way to forward declare, but commented out becauase I want to do the list at beginning strategy.
	UCapsuleComponent* Capsule;
	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* BirdMesh;
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* ViewCamera;

};

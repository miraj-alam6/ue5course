// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"
#include "Slash/DebugMacros.h"
#include "Kismet/KismetMathLibrary.h"


#define THIRTY 30

AItem::AItem() : Amplitude(5.0f), TimeConstant(2.f)
{
	PrimaryActorTick.bCanEverTick = true;

}

void AItem::BeginPlay()
{
	UE_LOG(LogTemp, Warning, TEXT("This happens before BP "))
	Super::BeginPlay(); //This calls BP BeginPlay. How is BP_Item a super class of Item?
	UE_LOG(LogTemp, Warning, TEXT("Begin Play called! This happens after BP, but I don't get why, isn't BP the sub, not the super"));
	if (GEngine) {
		GEngine->AddOnScreenDebugMessage(1, 60.f, FColor::Blue, FString("Item On screen message"));
	}
	//Note: this does not get reset between different PIE sessions which is why I do a modulo
	static int Incrementer = 0;
	Incrementer++;
	Incrementer = Incrementer % 4 + 2;
	SetActorLocation(FVector(50.f * Incrementer, 100.f * Incrementer, 50.f * Incrementer));
	SetActorRotation(FRotator(0.f, 45.f * Incrementer, 0.f));

	UWorld* World = GetWorld();
	FVector Location = GetActorLocation();

	UE_LOG(LogTemp, Warning, TEXT("Incrementer %i"), Incrementer);
	FVector Forward = GetActorForwardVector();

	RunningTime = 0.f;




}

void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	float RotationRate = 60.f; 

	RunningTime += DeltaTime;
	//float DeltaZ = Amplitude * FMath::Sin(RunningTime * TimeConstant);
	float DeltaZ = TransformedSine(RunningTime);
	//AddActorWorldOffset(FVector(0, 0, DeltaZ));	
	AddActorWorldRotation(FRotator(0, RotationRate * DeltaTime, 0));
	DRAW_SPHERE_SingleFrame(GetActorLocation());
	DRAW_VECTOR_SingleFrame(GetActorLocation(), GetActorLocation() + (GetActorForwardVector() * 100.f));
}

float AItem::TransformedSine(float InputValue) {
	return Amplitude * FMath::Sin(InputValue * TimeConstant);
}

float AItem::TransformedCosine(float InputValue) {
	return Amplitude * FMath::Cos(InputValue * TimeConstant);
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"
#include "Slash/DebugMacros.h"
#include "Kismet/KismetMathLibrary.h"


#define THIRTY 30

AItem::AItem() : Amplitude(5.0f), TimeConstant(2.f)
{
	PrimaryActorTick.bCanEverTick = true;
	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMeshComponent"));
	RootComponent = ItemMesh;
}

void AItem::BeginPlay()
{

	Super::BeginPlay(); //This calls BP BeginPlay. How is BP_Item a super class of Item?

	UWorld* World = GetWorld();
	FVector Location = GetActorLocation();
	FVector Forward = GetActorForwardVector();


	int32 intTest = Average<int32>(2, 5);
	float floatTest = Average<float>(2, 5);
	FVector vectorTest = Average<FVector>(FVector(2, 4, 9), FVector(5, 6, 7));

	RunningTime = 0.f;

	UE_LOG(LogTemp, Warning, TEXT("Averages %i   %f   %s"), intTest, floatTest, *(vectorTest.ToString()));

}

void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RunningTime += DeltaTime;	
}

float AItem::TransformedSine(float InputValue) {
	return Amplitude * FMath::Sin(InputValue * TimeConstant);
}

float AItem::TransformedCosine(float InputValue) {
	return Amplitude * FMath::Cos(InputValue * TimeConstant);
}

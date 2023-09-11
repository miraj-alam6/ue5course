// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"
#include "Slash/DebugMacros.h"
#include "Kismet/KismetMathLibrary.h"


#define THIRTY 30

AItem::AItem()
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
	UWorld* World = GetWorld();
	FVector Location = GetActorLocation();
	FVector Forward = GetActorForwardVector();
	//before making the macro I had this code:
	//if (World) {
	//	FVector Location = GetActorLocation();
	//	DrawDebugSphere(World, Location, 25.f, 24, FColor::Cyan, false, 30.f);
	//}

	//if (World) {
	//	FVector Forward = GetActorForwardVector();
	//	DrawDebugLine(GetWorld(), Location, Location + Forward * 100.f, FColor::Red, true, -1.f, 0, 1.f);
	//}
	
	//if (World) {
	//	DrawDebugPoint(World, Location + Forward * 100.f, 15.f, FColor::Red, true);
	//}

	//DRAW_SPHERE(Location);
	DRAW_COLOR_SPHERE(Location, FColor::Blue);
	//DRAW_LINE(Location, Location + Forward * 100.f);
	//DRAW_POINT(Location);
	DRAW_VECTOR(Location, Location + Forward * 100.f);

	////my own formula for drawing a box in between two points.
	//FVector StartLocation = GetActorLocation();
	//FVector EndLocation = GetActorLocation() + Forward * 400.f;
	//float Width = 10.f;
	//float Length = 10.f;
	////FVector blah = FVector(4, 4, 5); // variable not relevant for actual problem just a reminder you can initialize like this
	////float f = (float)FVector::Dist(EndLocation, StartLocation); // variable not relevant for actual problem just a reminder you need to cast
	//const FVector center = ((StartLocation + EndLocation) / 2.0);
	//const FVector extent = FVector((float)FVector::Dist(EndLocation, StartLocation), Length, Width) / 2.0;
	//const FQuat& quat = UKismetMathLibrary::MakeRotFromX((EndLocation - StartLocation).GetSafeNormal()).Quaternion();
	//DrawDebugBox(GetWorld(), center, extent, quat, FColor::Red, true, -1, 0, 5.f);
	DRAW_BOX_SPAN(Location, Location + Forward * 200.f, 10.f, 10.f);	
}

void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UE_LOG(LogTemp, Warning, TEXT("DeltaTime: %f"), DeltaTime)
	if (GEngine) {
		FString Name = GetName();
		FString Message = FString::Printf(TEXT("DeltaTime: %f Item Name %s." ),
			DeltaTime, *Name);
		//0 counts as a unique key, -1 means don't use any key
		GEngine->AddOnScreenDebugMessage(0, 60.f, FColor::Blue, Message);
		UE_LOG(LogTemp, Warning, TEXT("ItemName: %s"), *Name);
	}
}


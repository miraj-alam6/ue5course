// Fill out your copyright notice in the Description page of Project Settings.


#include "Breakable/BreakableActor.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Items/Treasure.h"
#include "Components/CapsuleComponent.h"
#include "PhysicsEngine/PhysicsObjectBlueprintLibrary.h"


// Sets default values
ABreakableActor::ABreakableActor()
{
	//Note how we don't need to tick
	PrimaryActorTick.bCanEverTick = false;
	GeometryCollection = CreateDefaultSubobject<UGeometryCollectionComponent>(
		(TEXT("GeometryCollection"))
	);

	SetRootComponent(GeometryCollection);
	GeometryCollection->SetGenerateOverlapEvents(true);
	GeometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GeometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	Capsule->SetupAttachment(GetRootComponent());
	Capsule->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	Capsule->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
}


void ABreakableActor::BeginPlay()
{
	Super::BeginPlay();
	
}


//Reminder: will never reach here
void ABreakableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABreakableActor::GetHit_Implementation(const FVector& ImpactPoint)
{
	UWorld* World = GetWorld();
	if (World && TreasureClass) {
		//Actor location does not actually represent where the breakableactor's physics objects are
		//but this is how the dude in the tutorial did it.
		//FVector Location = GetActorLocation();
		FVector Location = UPhysicsObjectBlueprintLibrary::GetPhysicsObjectWorldTransform(GeometryCollection, FName("None")).GetLocation();
		//Location.Z += 75.f;
		//World->SpawnActor<ATreasure>(TreasureClass, Location, GetActorRotation());

		//UPhysicsObjectBlueprintLibrary::GetClosestPhysicsObjectFromWorldLocation(GeometryCollection, GetActorLocation());
	}
	//Better to set ignore from Chaos Break Event in BP. But this means the stuff above also won't work all the time
	//If physics without hit breaks the pot, then the pot won't give the prize.
	//Capsule->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	//GetWorld()->SpawnActor();
}


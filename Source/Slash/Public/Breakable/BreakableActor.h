// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/HitInterface.h"
#include "BreakableActor.generated.h"

class UGeometryCollectionComponent;
class ATreasure;
class UCapsuleComponent;

UCLASS()
class SLASH_API ABreakableActor : public AActor, public IHitInterface
{
	GENERATED_BODY()

public:	
	ABreakableActor();
	virtual void Tick(float DeltaTime) override;
	//From IHitInterface old way
	//virtual void GetHit(const FVector& ImpactPoint) override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint) override;
protected:
	virtual void BeginPlay() override;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UGeometryCollectionComponent* GeometryCollection;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UCapsuleComponent* Capsule;
	UFUNCTION(BlueprintNativeEvent)
	FTransform GetPhysicsBodyWorkaround(UPrimitiveComponent* Component);
private:
	UPROPERTY(EditAnywhere, Category = "Breakable Properties")
	TArray<TSubclassOf<ATreasure>> Treasures;
	bool bBroken = false;
	//Using a UClass is possible but it's a bit too general, too many choices that leave a lot of room for error
	//when setting in editor. It's better to use a TSubclassOf
	//UClass* TreasureClass;
	// Old code when there was only one type of treasure
	//UPROPERTY(EditAnywhere, Category = "Breakable Properties")
	//TSubclassOf<ATreasure> TreasureClass;

public:
	UFUNCTION(BlueprintPure)
	UGeometryCollectionComponent* GetGeometryCollection() const { return GeometryCollection;}
};

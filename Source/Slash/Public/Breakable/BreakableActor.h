// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/HitInterface.h"
#include "BreakableActor.generated.h"

class UGeometryCollectionComponent;

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
private:
	UPROPERTY(VisibleAnywhere)
	UGeometryCollectionComponent* GeometryCollection;


public:
	UFUNCTION(BlueprintPure)
	UGeometryCollectionComponent* GetGeometryCollection() const { return GeometryCollection;}
};

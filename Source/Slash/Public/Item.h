// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"



UCLASS()
class SLASH_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	AItem();
	virtual void Tick(float DeltaTime) override;



protected:
	virtual void BeginPlay() override;
	//UPROPERTY(EditAnywhere, BlueprintReadonly)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sine Parameters")
	float Amplitude;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sine Parameters")
	float TimeConstant;
	//the dude removed InputValue as a parameter and forced to always use RunningTime, but I kept it
	//UFUNCTION(BlueprintCallable)
	UFUNCTION(BlueprintPure)
	float TransformedSine(float InputValue);
	UFUNCTION(BlueprintPure)
	float TransformedCosine(float InputValue);

	template<typename T>
	T Average (T First, T Second);
private:
	//Note how you can just make specifiers with just the defaults and not make it useable in event graph if that's what you wish
	//UPROPERTY(VisibleDefaultsOnly)
	//UPROPERTY(VisibleInstanceOnly)		
	//UPROPERTY(VisibleAnywhere)
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, meta = (AllowPrivateAccess = "true"))  //This line is the one that Stephen used, but I didn't think it was necessary
	float RunningTime;
	//UPROPERTY(EditDefaultsOnly)
	//UPROPERTY(EditAnywhere)
	//float Amplitude; 	
	//UPROPERTY(EditInstanceOnly)

};

template<typename T>
inline T AItem::Average(T First, T Second)
{
	return (First + Second) / 2;
}

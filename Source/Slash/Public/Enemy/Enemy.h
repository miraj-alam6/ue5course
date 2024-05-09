// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "Characters/CharacterTypes.h"

#include "Enemy.generated.h"

class UAnimMontage;
class AAIController;
class UAttributeComponent;
class UHealthBarComponent;
class UPawnSensingComponent;

UCLASS()
class SLASH_API AEnemy : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	AEnemy();
	virtual void Tick(float DeltaTime) override;

	void CheckPatrolTarget();

	void CheckCombatTarget();

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	//From IHitInterface old way
	//virtual void GetHit(const FVector& ImpactPoint) override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint) override;
	void DirectionalHitReact(const FVector& ImpactPoint);
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

protected:
	virtual void BeginPlay() override;

	void Die();
	/**
	* Play Montage Functions
	*
	*/
	void PlayHitReactMontage(const FName& SectionName);

	UPROPERTY(BlueprintReadOnly)
	EDeathPose DeathPose = EDeathPose::EDP_Alive;

	//Can be used for both combat and patrol targets.
	bool InTargetRange(AActor* Target, double Radius);
	void MoveToTarget(AActor* Target);
	AActor* ChoosePatrolTarget();

	UFUNCTION()
	void OnPawnSeen(APawn* SeenPawn);

	EEnemyState EnemyState = EEnemyState::EES_Patrolling;
private:

	/*
	* Components
	*/

	UPROPERTY(VisibleAnywhere)
	UAttributeComponent* Attributes;
	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent* HealthBarWidget;
	UPROPERTY(VisibleAnywhere)
	UPawnSensingComponent* PawnSensing;
/**
* Animation montages
*/
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* HitReactMontage;
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* DeathMontage;

	UPROPERTY(EditAnywhere, Category = Sounds)
	USoundBase* HitSound;

	UPROPERTY(EditAnywhere, Category = VisualEffects)
	UParticleSystem* HitParticles;

	UPROPERTY()
	AActor* CombatTarget;

	UPROPERTY(EditAnywhere)
	double CombatRadius = 500.f;
	UPROPERTY(EditAnywhere)
	double AttackRadius = 150.f;
	//Will be overriden before doing MoveTo
	UPROPERTY(EditAnywhere)
	double PatrolDoneRadius = -1.f;


	FTimerHandle PatrolTimer;
	void PatrolTimerFinished();


	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float WaitMin = 2.f;
	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float WaitMax = 8.f;

	const float AcceptanceRadius = 15.f;

	/**
	*Navigation 
	*/
	//Current Patrol Target
	UPROPERTY()
	AAIController* EnemyController;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	AActor* PatrolTarget;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TArray<AActor*> PatrolTargets;
public:	


};

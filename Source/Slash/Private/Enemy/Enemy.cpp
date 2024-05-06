// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Components/AttributeComponent.h"
#include "HUD/HealthBarComponent.h"

#include "Animation/AnimMontage.h"
#include "Animation/AnimInstance.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "AIController.h"

#include "Slash/DebugMacros.h"


AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	GetCharacterMovement()->bAllowPhysicsRotationDuringAnimRootMotion = false;


	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	//Note how UAttributeComponent does not need to be attached to anything because it's just data.
	//However the transform for HealthBarWidget does matter, so we do attach it.
	Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attributes"));
	HealthBarWidget = CreateDefaultSubobject<UHealthBarComponent>(TEXT("HealthBar"));
	HealthBarWidget->SetupAttachment(GetRootComponent());


	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));	
	PawnSensing->SightRadius = 4000.f;
	PawnSensing->SetPeripheralVisionAngle(45.f);

	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	if (HealthBarWidget) {
		HealthBarWidget->SetVisibility(false);
	}

	if (HealthBarWidget) {
		HealthBarWidget->SetHealthPercent(1.f);
	}

	EnemyController = Cast<AAIController>(GetController());
	MoveToTarget(PatrolTarget);

	if (PawnSensing) {
		PawnSensing->OnSeePawn.AddDynamic(this, &AEnemy::OnPawnSeen);
	}
}

void AEnemy::Die()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && DeathMontage) {
		AnimInstance->Montage_Play(DeathMontage);


		//How the dude did the section Name. 
		// I commented it out and  feel like I made an improved more elegant solution.
		// const int32 Selection = FMath::RandRange(0, 5);
		//FName SectionName = FName();
		//switch (Selection) {
		//case 0:
		//	SectionName = FName("Death1");
		//  DeathPose = EDeathPose::EDP_Death1
		//	break;
		//case 1:
		//	SectionName = FName("Death2");
		//  DeathPose = EDeathPose::EDP_Death2
		//	break;
		//case 2:
		//	SectionName = FName("Death3");
		//	DeathPose = EDeathPose::EDP_Death3
		//	break;
		//case 3:
		//	SectionName = FName("Death4");
		//	DeathPose = EDeathPose::EDP_Death4
		//	break;
		//case 4:
		//	SectionName = FName("Death5");
		//	DeathPose = EDeathPose::EDP_Death5
		//	break;
		//case 5:
		//	SectionName = FName("Death6");
		//	DeathPose = EDeathPose::EDP_Death6
		//	break;
		//default:
		//	SectionName = FName("Death1");
		//}

		//How I did it
		const int32 Selection = FMath::RandRange(0, 5) + 1;
		//I made sure that EDeathPose values correspond to explicit integer values
		DeathPose = (EDeathPose)(Selection);
		FName SectionName =  FName(FString::Printf(TEXT("Death%i"), Selection));
		AnimInstance->Montage_JumpToSection(SectionName, DeathMontage);
	}

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetLifeSpan(5.f);
	if (HealthBarWidget) {
		HealthBarWidget->SetVisibility(false);
	}
}

void AEnemy::PlayHitReactMontage(const FName& SectionName)
{

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage) {
		AnimInstance->Montage_Play(HitReactMontage);
		AnimInstance->Montage_JumpToSection(SectionName, HitReactMontage);
	}
}

bool AEnemy::InTargetRange(AActor* Target, double Radius)
{
	if (Target == nullptr) {
		return false;
	}

	const double DistanceToTarget = (Target->GetActorLocation() - GetActorLocation()).Size();
	DRAW_SPHERE_SingleFrame(GetActorLocation());
	DRAW_SPHERE_SingleFrame(Target->GetActorLocation());
	return DistanceToTarget <= Radius;
}

void AEnemy::MoveToTarget(AActor* Target)
{
	if (EnemyController == nullptr 
		|| Target == nullptr) {
		return;
	}
	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalActor(Target);
	MoveRequest.SetAcceptanceRadius(AcceptanceRadius);
	EnemyController->MoveTo(MoveRequest);
	
	//Not sure if I need to do the following PatrolDoneRadius stuff.
	//Give leeway so that PatrolDone gets detected before MoveTo can finish.
	PatrolDoneRadius = AcceptanceRadius + 150.f;

	//Version of MoveTo with debug spheres.
	//FNavPathSharedPtr NavPath;
	//TArray<FNavPathPoint>& PathPoints = NavPath->GetPathPoints();
	//for (auto& Point : PathPoints) {
	//	const FVector& Location = Point.Location;
	//	DrawDebugSphere(GetWorld(), Location, 12.f, 12, FColor::Green, false, 10.f);
	//}
	//EnemyController->MoveTo(MoveRequest, &NavPath);
}

AActor* AEnemy::ChoosePatrolTarget()
{
	TArray<AActor*> ValidNextTargets;
	for (AActor* Target : PatrolTargets) {
		if (Target != PatrolTarget) {
			ValidNextTargets.AddUnique(Target);
		}
	}
	//Select a new random Patrol Target.
	const int32 NumPatrolTargets = ValidNextTargets.Num();
	if (NumPatrolTargets > 0) {
		const int32 TargetSelection = FMath::RandRange(0, NumPatrolTargets - 1);
		return ValidNextTargets[TargetSelection];
	}

	return nullptr;
}

void AEnemy::OnPawnSeen(APawn* SeenPawn)
{
	//We will not go with the casting strategy here, instead we will use ActorTag checking
	//In order to save on performance.
	//if (Cast<ASlashCharacter>(SeenPawn)) {
	//}

	if (EnemyState == EEnemyState::EES_Chasing) {
		return;
	}
	if (SeenPawn->ActorHasTag(FName("SlashCharacter"))) {		
		EnemyState = EEnemyState::EES_Chasing;
		GetWorldTimerManager().ClearTimer(PatrolTimer);
		//Hard coded for now, we'll fix this later.
		GetCharacterMovement()->MaxWalkSpeed = 300.f;
		CombatTarget = SeenPawn;
		MoveToTarget(CombatTarget);
	}
}

void AEnemy::PatrolTimerFinished()
{
	MoveToTarget(PatrolTarget);
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Interesting strategy using enum's integer value as a gradient of aggressive escalation. 
	//Not sure if this is worth doing though, a lot of room for error because people won't realize this double purpose of the states.
	//This if statement essentially means: are you more aggressive than patrol state.
	if (EnemyState > EEnemyState::EES_Patrolling) {
		CheckCombatTarget();
	}
	else {
		CheckPatrolTarget();
	}
}

void AEnemy::CheckPatrolTarget()
{
	//NOTE: followed the tutorial, but this way of switching patrol targets is not smart. His strategy is to check if patrol target is in range,  
	//inside of tick, but MoveTo already has its own detection for when it finishes reaching a point, It would be better to piggyback off of that 
	// since there is probably a delegate we can bind to for finishing movement, rather than checking this every frame in Tick
	if (InTargetRange(PatrolTarget, PatrolDoneRadius)) {
		PatrolTarget = ChoosePatrolTarget();
		const float WaitTime = FMath::RandRange(WaitMin, WaitMax);
		GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemy::PatrolTimerFinished, WaitTime);
	}
}

void AEnemy::CheckCombatTarget()
{
	//We want to clear combat target if no longer in range.
	if (!InTargetRange(CombatTarget, CombatRadius)) {
		CombatTarget = nullptr;
		if (HealthBarWidget) {
			HealthBarWidget->SetVisibility(false);
		}

		//Reminder we only got to this function call if current state is non patrolling,
		// But then readching this inner loop means we got a little far away from the enemy,
		// So we should change the state to not be chase anymore. 

		EnemyState = EEnemyState::EES_Patrolling;
		GetCharacterMovement()->MaxWalkSpeed = 125.f;
		MoveToTarget(PatrolTarget);
	}
}

void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::GetHit_Implementation(const FVector& ImpactPoint)
{
	if (HealthBarWidget) {
		HealthBarWidget->SetVisibility(true);
	}

	if (Attributes && Attributes->IsAlive()) {
		DirectionalHitReact(ImpactPoint);
	}
	else {
		Die();
	}



	if (HitSound) {
		UGameplayStatics::PlaySoundAtLocation(this, HitSound, ImpactPoint);
	}
	//He said the GetWorld() is not null check is not necessary so I excluded it.
	if (HitParticles /*&& GetWorld()*/) {
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			HitParticles,
			ImpactPoint
		);
	}
}

void AEnemy::DirectionalHitReact(const FVector& ImpactPoint)
{

	const FVector Forward = GetActorForwardVector();
	//Making the Z be the same as actor so that dot product doesn't include Z component
	const FVector ImpactLowered(ImpactPoint.X, ImpactPoint.Y, GetActorLocation().Z);
	const FVector ToHit = (ImpactLowered - GetActorLocation()).GetSafeNormal();

	// Forward * ToHit = |Forward| |ToHit| * cos (theta) and because we normalize it is
	// Forward * ToHit =  cos (theta)
	const double CosTheta = FVector::DotProduct(Forward, ToHit);
	double Theta = FMath::Acos(CosTheta);
	//Convert from radians to degrees
	Theta = FMath::RadiansToDegrees(Theta);

	//If cross product points down, then we're being hit from the left and theta should be made negative
	const FVector CrossProduct = FVector::CrossProduct(Forward, ToHit);
	//Remember theta is always positive so we check if cross product indicates hit was from left, thus meaning we should
	//multiply theta by -1 to reflect the leftness since right is the positive direction.
	if (CrossProduct.Z < 0) {
		Theta *= -1.f;
	}

	//This is implictly the 135 to -135 case if none of the other ones succeed
	FName Section("FromBack");

	if (Theta >= -45.f && Theta < 45.f) {
		Section = FName("FromFront");
	}
	else if (Theta >= -135.f && Theta < -45.f) {
		Section = FName("FromLeft");
	}
	else if (Theta >= 45.f && Theta < 135.f) {
		Section = FName("FromRight");
	}

	PlayHitReactMontage(Section);
	
	////DEBUG WHEN HITTING
	//DRAW_SPHERE_COLOR(ImpactPoint, FColor::Orange);
	//UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + Forward * 60.f, 5.f, FColor::Red, 5.f);
	//if (GEngine) {
	//	GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Green, FString::Printf(TEXT("Theta %f"), Theta));
	//}
	//UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + CrossProduct * 100.f, 5.f, FColor::Blue, 5.f);
	//UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + Forward * 60.f, 5.f, FColor::Red, 5.f);
	//UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + ToHit * 60.f, 5.f, FColor::Green, 5.f);
	
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	//The dude's code is a little different, I did it differently on his purpose
	//This is his code:
	//if (Attributes && HealthBarWidget) {
	//	Attributes->ReceiveDamage(DamageAmount);
	//	HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());
	//}

	if (Attributes) {
		Attributes->ReceiveDamage(DamageAmount);
	}

	if (HealthBarWidget) {		
		HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());
	}

	CombatTarget = EventInstigator->GetPawn();
	return DamageAmount;
}


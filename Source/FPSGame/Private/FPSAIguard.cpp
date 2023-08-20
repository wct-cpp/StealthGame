// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSAIguard.h"
#include "Perception/PawnSensingComponent.h"
#include "DrawDebugHelpers.h"
#include "Components/PawnNoiseEmitterComponent.h"
#include "FPSGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Net/UnrealNetwork.h"


// Sets default values
AFPSAIguard::AFPSAIguard()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));

	PawnSensingComp->OnSeePawn.AddDynamic(this, &AFPSAIguard::OnPawnSeen);
	PawnSensingComp->OnHearNoise.AddDynamic(this, &AFPSAIguard::OnNoiseHeard);

	GuardState = EAIState::Idle;



	
}

// Called when the game starts or when spawned
void AFPSAIguard::BeginPlay()
{
	Super::BeginPlay();
	
	OriginalRotation = GetActorRotation();

	if (bPatrol) {
		MoveToNextPatrolPoint();
	}
}

void AFPSAIguard::OnPawnSeen(APawn* SeenPawn)
{
	if (SeenPawn == nullptr) {
		return;
	}
	DrawDebugSphere(GetWorld(), SeenPawn->GetActorLocation(),32.0f,12,FColor::Red,false,10.0f);

	AFPSGameMode* GM = Cast<AFPSGameMode>(GetWorld()->GetAuthGameMode());
	if (GM) {
		GM->CompleteMission(SeenPawn, false);
	}

	SetGuardState(EAIState::Alerted);

	//在巡逻时停止移动
	AController* Controller = GetController();
	if (Controller) {
		Controller->StopMovement();
	}
}

void AFPSAIguard::OnNoiseHeard(APawn* NoiseInstigator, const FVector& Location, float Volume)
{
	if (GuardState == EAIState::Alerted) {//Alerted优先级最高
		return;
	}

	DrawDebugSphere(GetWorld(), Location, 32.0f, 12, FColor::Green, false, 10.0f);

	FVector Direction = Location - GetActorLocation();//发出声音的地方-ai所在的地方
	Direction.Normalize();//如果该向量大于给定的公差，则在适当的位置对其进行规格化。否则保持不变。

	FRotator NewLookAt= FRotationMatrix::MakeFromX(Direction).Rotator();//在只给定X轴的情况下构建旋转矩阵。Y和Z未指定，但将是正交的。X轴无需规范化。
	NewLookAt.Pitch = 0.0f;
	NewLookAt.Roll = 0.0f;

	SetActorRotation(NewLookAt);

	GetWorldTimerManager().ClearTimer(TimerHandle_ResetOriention);//清除定时器

	GetWorldTimerManager().SetTimer(TimerHandle_ResetOriention, this, &AFPSAIguard::ResetOrientation, 3.0f);//设置定时器

	SetGuardState(EAIState::Suspicious);

	//在巡逻时停止移动
	AController* Controller = GetController();
	if (Controller) {
		Controller->StopMovement();
	}

}

void AFPSAIguard::ResetOrientation()
{
	if (GuardState == EAIState::Alerted) {//Alerted优先级最高
		return;
	}
	SetActorRotation(OriginalRotation);
	SetGuardState(EAIState::Idle);
	
	if (bPatrol) {
		MoveToNextPatrolPoint();
	}
}

void AFPSAIguard::OnRep_GuardState()
{
	OnStateChanged(GuardState);
}

void AFPSAIguard::SetGuardState(EAIState NewState)
{
	if (GuardState == NewState) {
		return;
	}

	GuardState = NewState;
	OnRep_GuardState();
}

void AFPSAIguard::MoveToNextPatrolPoint()
{
	if (CurrentPatrolPoint == nullptr || CurrentPatrolPoint == SecondPatrolPoint) {
		CurrentPatrolPoint = FirstPatrolPoint;
	}
	else {
		CurrentPatrolPoint = SecondPatrolPoint;
	}
	UAIBlueprintHelperLibrary::SimpleMoveToActor(GetController(), CurrentPatrolPoint);
}

// Called every frame
void AFPSAIguard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//巡逻目标更新
	if (CurrentPatrolPoint) {
		FVector Delta = GetActorLocation() - CurrentPatrolPoint->GetActorLocation();
		float DistanceToGoal = Delta.Size();

		if (DistanceToGoal < 100) {
			MoveToNextPatrolPoint();
		}
	}
}

//GuardState同步规则
void AFPSAIguard::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFPSAIguard, GuardState);//GuardState所有客户端无条件全同步
	//DOREPLIFETIME_CONDITION(AFPSAIguard, GuardState, COND_OwnerOnly);一定条件下同步
}
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

	//��Ѳ��ʱֹͣ�ƶ�
	AController* Controller = GetController();
	if (Controller) {
		Controller->StopMovement();
	}
}

void AFPSAIguard::OnNoiseHeard(APawn* NoiseInstigator, const FVector& Location, float Volume)
{
	if (GuardState == EAIState::Alerted) {//Alerted���ȼ����
		return;
	}

	DrawDebugSphere(GetWorld(), Location, 32.0f, 12, FColor::Green, false, 10.0f);

	FVector Direction = Location - GetActorLocation();//���������ĵط�-ai���ڵĵط�
	Direction.Normalize();//������������ڸ����Ĺ�������ʵ���λ�ö�����й�񻯡����򱣳ֲ��䡣

	FRotator NewLookAt= FRotationMatrix::MakeFromX(Direction).Rotator();//��ֻ����X�������¹�����ת����Y��Zδָ���������������ġ�X������淶����
	NewLookAt.Pitch = 0.0f;
	NewLookAt.Roll = 0.0f;

	SetActorRotation(NewLookAt);

	GetWorldTimerManager().ClearTimer(TimerHandle_ResetOriention);//�����ʱ��

	GetWorldTimerManager().SetTimer(TimerHandle_ResetOriention, this, &AFPSAIguard::ResetOrientation, 3.0f);//���ö�ʱ��

	SetGuardState(EAIState::Suspicious);

	//��Ѳ��ʱֹͣ�ƶ�
	AController* Controller = GetController();
	if (Controller) {
		Controller->StopMovement();
	}

}

void AFPSAIguard::ResetOrientation()
{
	if (GuardState == EAIState::Alerted) {//Alerted���ȼ����
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

	//Ѳ��Ŀ�����
	if (CurrentPatrolPoint) {
		FVector Delta = GetActorLocation() - CurrentPatrolPoint->GetActorLocation();
		float DistanceToGoal = Delta.Size();

		if (DistanceToGoal < 100) {
			MoveToNextPatrolPoint();
		}
	}
}

//GuardStateͬ������
void AFPSAIguard::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFPSAIguard, GuardState);//GuardState���пͻ���������ȫͬ��
	//DOREPLIFETIME_CONDITION(AFPSAIguard, GuardState, COND_OwnerOnly);һ��������ͬ��
}
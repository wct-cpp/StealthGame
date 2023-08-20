// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FPSAIguard.generated.h"

class UPawnSensingComponent;
class UPawnNoiseEmitterComponent;


//AI״̬ö��
UENUM(BlueprintType)
enum class EAIState : uint8 {//��������ͼ��Ҫ��uint8
	Idle,

	Suspicious,

	Alerted
};

UCLASS()
class FPSGAME_API AFPSAIguard : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AFPSAIguard();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere,Category="Components")
	UPawnSensingComponent* PawnSensingComp;

	UFUNCTION()
	void OnPawnSeen(APawn* SeenPawn);

	UFUNCTION()
	void OnNoiseHeard(APawn* NoiseInstigator, const FVector& Location, float Volume);

	FRotator OriginalRotation;//��ʼ����

	UFUNCTION()
	void ResetOrientation();//���÷���

	FTimerHandle TimerHandle_ResetOriention;//��ʱ�����

	UPROPERTY(ReplicatedUsing=OnRep_GuardState)//ReplicatedUsing�󶨵ĺ������ڿͻ��˵ı������յ��������ĸı�ʱ���ڿͻ������Զ����á�����˲����Զ����ã���Ҫͨ�������ֶ����á�
	EAIState GuardState;//����״̬

	UFUNCTION()
	void OnRep_GuardState();

	void SetGuardState(EAIState NewState);//���þ���״̬

	UFUNCTION(BlueprintImplementableEvent,Category="AI")
	void OnStateChanged(EAIState NewState);//��ͼ�е���

	UPROPERTY(EditInstanceOnly,Category="AI")
	bool bPatrol;

	UPROPERTY(EditInstanceOnly,Category="AI",meta=(EditCondition="bPatrol"))
	AActor* FirstPatrolPoint;
	UPROPERTY(EditInstanceOnly,Category="AI",meta=(EditCondition="bPatrol"))
	AActor* SecondPatrolPoint;

	AActor* CurrentPatrolPoint;

	void MoveToNextPatrolPoint();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};

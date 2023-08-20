// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FPSAIguard.generated.h"

class UPawnSensingComponent;
class UPawnNoiseEmitterComponent;


//AI状态枚举
UENUM(BlueprintType)
enum class EAIState : uint8 {//公开给蓝图需要用uint8
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

	FRotator OriginalRotation;//初始方向

	UFUNCTION()
	void ResetOrientation();//重置方向

	FTimerHandle TimerHandle_ResetOriention;//定时器句柄

	UPROPERTY(ReplicatedUsing=OnRep_GuardState)//ReplicatedUsing绑定的函数是在客户端的变量接收到服务器的改变时，在客户端上自动调用。服务端不会自动调用，需要通过代码手动调用。
	EAIState GuardState;//警卫状态

	UFUNCTION()
	void OnRep_GuardState();

	void SetGuardState(EAIState NewState);//设置警卫状态

	UFUNCTION(BlueprintImplementableEvent,Category="AI")
	void OnStateChanged(EAIState NewState);//蓝图中调用

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

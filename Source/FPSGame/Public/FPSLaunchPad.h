// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FPSLaunchPad.generated.h"


class UBoxComponent;

UCLASS()
class FPSGAME_API AFPSLaunchPad : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFPSLaunchPad();

protected:

	UPROPERTY(VisibleAnywhere,Category="Components")
	UStaticMeshComponent* MeshComp;
	UPROPERTY(VisibleAnywhere,Category="Components")
	UBoxComponent* OverlapComp;

	UFUNCTION()
	void HandleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);



	UPROPERTY(EditDefaultsOnly,Category="LaunchPad")
	float LaunchStrength;

	UPROPERTY(EditDefaultsOnly,Category="LaunchPad")
	float LaunchPitchAngle;

	UPROPERTY(EditDefaultsOnly,Category="LaunchPad")
	UParticleSystem* ActivateLaunchPadEffect;
};

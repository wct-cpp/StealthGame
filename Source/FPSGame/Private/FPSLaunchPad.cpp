// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSLaunchPad.h"
#include "Components/BoxComponent.h"
#include "FPSCharacter.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AFPSLaunchPad::AFPSLaunchPad()
{

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	OverlapComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	RootComponent = OverlapComp;
	MeshComp->SetupAttachment(RootComponent);

	OverlapComp->SetBoxExtent(FVector(90.0f, 90.0f, 100.0f));

	OverlapComp->OnComponentBeginOverlap.AddDynamic(this,&AFPSLaunchPad::HandleOverlap);

	LaunchStrength = 1500;
	LaunchPitchAngle = 35.0f;
}


void AFPSLaunchPad::HandleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	FRotator LaunchDirection = GetActorRotation();
	LaunchDirection.Pitch += LaunchPitchAngle;
	FVector LaunchVelocity = LaunchDirection.Vector() * LaunchStrength;

	ACharacter* OtherCharacter = Cast<ACharacter>(OtherActor);
	if (OtherCharacter) {
		//Launch player
		OtherCharacter->LaunchCharacter(LaunchVelocity, false, false);

		//Spawn FX
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ActivateLaunchPadEffect, GetActorLocation());
	}
	else if (OtherComp && OtherComp->IsSimulatingPhysics()) {
		OtherComp->AddImpulse(LaunchVelocity, NAME_None,true);

		//Spawn FX
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ActivateLaunchPadEffect, GetActorLocation());
	}
}





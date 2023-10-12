// Fill out your copyright notice in the Description page of Project Settings.


#include "ARPlayer.h"
#include "Components/CapsuleComponent.h"
#include <GameFramework/Pawn.h>

// Sets default values
AARPlayer::AARPlayer()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("BoxComp"));
	RootComponent = BoxComp;
	BoxComp->SetCapsuleHalfHeight(50.0f);
	BoxComp->SetCapsuleRadius(26.0f);

	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
	BodyMesh->SetupAttachment(RootComponent);

	ConstructorHelpers::FObjectFinder<UStaticMesh> TempMesh(TEXT("/Script/Engine.StaticMesh'/Game/StarterContent/Shapes/Shape_NarrowCapsule.Shape_NarrowCapsule'"));

	if (TempMesh.Succeeded())
	{
		BodyMesh->SetStaticMesh(TempMesh.Object);
	}


}

// Called when the game starts or when spawned
void AARPlayer::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AARPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	FVector p0 = GetActorLocation();
	FVector vt = Dir * speed * DeltaTime;
	FVector p = p0 + vt;
	SetActorLocation(p, true);

}

// Called to bind functionality to input
void AARPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("Horizontal"), this, &AARPlayer::Horizontal);
	PlayerInputComponent->BindAxis(TEXT("Vertical"), this, &AARPlayer::Vertical);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &AARPlayer::Turn);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &AARPlayer::LookUp);
}

void AARPlayer::Horizontal(float value)
{
	Dir.Y = value;
}

void AARPlayer::Vertical(float value)
{
	Dir.X = value;
}

void AARPlayer::Turn(float value)
{
	AddControllerYawInput(value);
}

void AARPlayer::LookUp(float value)
{
	AddControllerPitchInput(value);
}

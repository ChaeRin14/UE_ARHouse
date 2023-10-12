// Fill out your copyright notice in the Description page of Project Settings.


#include "ARCharacter.h"
#include "Components/CapsuleComponent.h"
#include <GameFramework/Pawn.h>
#include <Camera/CameraComponent.h>
#include <GameFramework/SpringArmComponent.h>

// Sets default values
AARCharacter::AARCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//BoxComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("BoxComp"));
	//RootComponent = BoxComp;
	//BoxComp->SetCapsuleHalfHeight(50.0f);
	//BoxComp->SetCapsuleRadius(26.0f);

	//BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
	//BodyMesh->SetupAttachment(RootComponent);

	//ConstructorHelpers::FObjectFinder<UStaticMesh> TempMesh(TEXT("/Script/Engine.StaticMesh'/Game/StarterContent/Shapes/Shape_NarrowCapsule.Shape_NarrowCapsule'"));

	//if (TempMesh.Succeeded())
	//{
	//	BodyMesh->SetStaticMesh(TempMesh.Object);
	//}

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(RootComponent);
	arCamComp = CreateDefaultSubobject<UCameraComponent>(TEXT("arCamComp"));
	arCamComp->SetupAttachment(SpringArmComp);

}

// Called when the game starts or when spawned
void AARCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AARCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector p0 = GetActorLocation();
	FVector vt = Dir * speed * DeltaTime;
	FVector p = p0 + vt;
	SetActorLocation(p, true);

}

// Called to bind functionality to input
void AARCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);


	PlayerInputComponent->BindAxis(TEXT("Horizontal"), this, &AARCharacter::Horizontal);
	PlayerInputComponent->BindAxis(TEXT("Vertical"), this, &AARCharacter::Vertical);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &AARCharacter::Turn);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &AARCharacter::LookUp);
}


void AARCharacter::Horizontal(float value)
{
	AddMovementInput(GetActorRightVector(), value);
}

void AARCharacter::Vertical(float value)
{
	AddMovementInput(GetActorForwardVector(), value);
}

void AARCharacter::Turn(float value)
{
	AddControllerYawInput(value);
}

void AARCharacter::LookUp(float value)
{
	AddControllerPitchInput(value);
}


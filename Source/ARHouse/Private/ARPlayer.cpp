// Fill out your copyright notice in the Description page of Project Settings.


#include "ARPlayer.h"
#include "Components/CapsuleComponent.h"
#include <GameFramework/Pawn.h>
#include <Camera/CameraComponent.h>
#include <GameFramework/SpringArmComponent.h>
/*#include "ARBlueprintLibrary.h"*/

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

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(BoxComp);
	arCamComp = CreateDefaultSubobject<UCameraComponent>(TEXT("arCamComp"));
	arCamComp->SetupAttachment(SpringArmComp);

}

// Called when the game starts or when spawned
void AARPlayer::BeginPlay()
{
	Super::BeginPlay();
	
// 	pc = GetController<APlayerController>();
// 
// 	// AR 카메라 켜기
// 	UARBlueprintLibrary::SetEnabledXRCamera(true);
// 
// 	// 설정된 값으로 AR 세션 실행
// 	UARBlueprintLibrary::StartARSession(arSessionCinfig1);
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

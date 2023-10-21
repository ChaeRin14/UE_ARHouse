// Fill out your copyright notice in the Description page of Project Settings.


#include "RotationArrrowActor.h"
#include <Components/BoxComponent.h>

// Sets default values
ARotationArrrowActor::ARotationArrrowActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	boxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	RootComponent = boxComp;
	boxComp->SetBoxExtent(FVector(70, 70, 20));
	boxComp->SetCollisionProfileName(TEXT("RotArrow"));

	bodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("bodyMesh"));
	bodyMesh->SetupAttachment(boxComp);
	bodyMesh->SetRelativeLocation(FVector(0, -10, 0));
	bodyMesh->SetRelativeScale3D(FVector(0.5f));

	ConstructorHelpers::FObjectFinder<UStaticMesh> TempMesh(TEXT("/Script/Engine.StaticMesh'/Game/Asset/Arrow/StaticMesh.StaticMesh'"));

	if (TempMesh.Succeeded())
	{
		bodyMesh->SetStaticMesh(TempMesh.Object);
	}

}

// Called when the game starts or when spawned
void ARotationArrrowActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARotationArrrowActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


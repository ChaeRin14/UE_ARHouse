// Fill out your copyright notice in the Description page of Project Settings.


#include "Bed.h"
#include <Components/BoxComponent.h>

// Sets default values
ABed::ABed()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	RootComponent = BoxComp;
	BoxComp->SetBoxExtent(FVector(140, 120, 50));
	BoxComp->SetCollisionProfileName(TEXT("Bed"));

	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
	BodyMesh->SetupAttachment(BoxComp);

	BodyMesh->SetRelativeLocation(FVector(0, 10, -50));

	ConstructorHelpers::FObjectFinder<UStaticMesh> TempMesh(TEXT("/Script/Engine.StaticMesh'/Game/Asset/bed/Bed.Bed'"));

	if (TempMesh.Succeeded())
	{
		BodyMesh->SetStaticMesh(TempMesh.Object);
	}

}

// Called when the game starts or when spawned
void ABed::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABed::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}




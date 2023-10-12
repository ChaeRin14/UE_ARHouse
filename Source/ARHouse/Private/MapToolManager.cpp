// Fill out your copyright notice in the Description page of Project Settings.


#include "MapToolManager.h"
#include <Kismet/GameplayStatics.h>
#include "ARCharacter.h"
#include "Bed.h"

// Sets default values
AMapToolManager::AMapToolManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMapToolManager::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AMapToolManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FActorSpawnParameters param;
	param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	player = Cast<AARCharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), AARCharacter::StaticClass()));

	FVector SpawnLocation = player->GetActorLocation();
	SpawnLocation.Normalize();
	SpawnLocation.X  *= 700;
	SpawnLocation.Z = 50;

	if (isBedSpawn)
	{
		bedActor = GetWorld()->SpawnActor<ABed>(SpawnLocation, GetActorRotation(), param);

		isBedSpawn = false;
	}
}


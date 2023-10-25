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

}


// Fill out your copyright notice in the Description page of Project Settings.


#include "Light_PointLight.h"

// Sets default values
ALight_PointLight::ALight_PointLight()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ALight_PointLight::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALight_PointLight::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


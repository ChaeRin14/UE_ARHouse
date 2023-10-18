// Fill out your copyright notice in the Description page of Project Settings.


#include "HttpRequestActor.h"
#include <HttpModule.h>
#include <ImageUtils.h>
#include <DesktopPlatform/Public/IDesktopPlatform.h>
#include <DesktopPlatform/Public/DesktopPlatformModule.h>
#include "../ARHouseGameModeBase.h"

// Sets default values
AHttpRequestActor::AHttpRequestActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AHttpRequestActor::BeginPlay()
{
	Super::BeginPlay();
	gm = GetWorld()->GetAuthGameMode<AARHouseGameModeBase>();
	
}

// Called every frame
void AHttpRequestActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

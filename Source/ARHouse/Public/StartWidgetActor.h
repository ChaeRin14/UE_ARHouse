// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "StartWidgetActor.generated.h"

UCLASS()
class ARHOUSE_API AStartWidgetActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AStartWidgetActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = MySettings)
	TSubclassOf<AActor> indicator;

	UPROPERTY(EditAnywhere, Category = MySettings)
	TSubclassOf<UUserWidget> widget_BP; 

private:
	AActor* spawnedIndicator;
	UUserWidget* widget_inst;

	void SetIndicator();
};

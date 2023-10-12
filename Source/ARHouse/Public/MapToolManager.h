// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Bed.h"
#include "MapToolManager.generated.h"

UCLASS()
class ARHOUSE_API AMapToolManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMapToolManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	class AARCharacter* player;

	class ABed* bedActor;

	UPROPERTY(BlueprintReadWrite, Category="ActorSpawnd")
	bool isBedSpawn = false;


	UPROPERTY(BlueprintReadWrite, Category="MoveKey")
	bool isWClick = false;

	UPROPERTY(BlueprintReadWrite, Category="MoveKey")
	bool isAClick = false;

	UPROPERTY(BlueprintReadWrite, Category="MoveKey")
	bool isSClick = false;

	UPROPERTY(BlueprintReadWrite, Category="MoveKey")
	bool isDClick = false;

};

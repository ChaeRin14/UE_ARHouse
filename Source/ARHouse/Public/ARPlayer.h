// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "ARPlayer.generated.h"

UCLASS()
class ARHOUSE_API AARPlayer : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AARPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	float speed = 500;
	FVector Dir;

	UPROPERTY(EditAnywhere)
	class UCapsuleComponent* BoxComp;
	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* BodyMesh;

	void Horizontal(float value);
	void Vertical(float value);

	void Turn(float value);
	void LookUp(float value);

};

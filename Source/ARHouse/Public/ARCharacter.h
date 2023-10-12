// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ARCharacter.generated.h"

UCLASS()
class ARHOUSE_API AARCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AARCharacter();

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

	UPROPERTY(EditAnywhere, Category = MySettings)
	class UCapsuleComponent* BoxComp;
	
	UPROPERTY(EditAnywhere, Category = MySettings)
	class UStaticMeshComponent* BodyMesh;
	
	UPROPERTY(EditAnywhere, Category = MySettings)
	class USpringArmComponent* SpringArmComp;
	
	UPROPERTY(EditAnywhere, Category = MySettings)
	class UCameraComponent* arCamComp;

	UPROPERTY(EditAnywhere, Category = MySettings)
	class APlayerController* pc;

	UPROPERTY(EditAnywhere, Category = MySettings)
	class UARSessionConfig* arSessionCinfig1;

	void Horizontal(float value);
	void Vertical(float value);

	void Turn(float value);
	void LookUp(float value);

};

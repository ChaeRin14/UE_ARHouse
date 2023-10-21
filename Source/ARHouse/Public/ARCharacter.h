// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
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

	UPROPERTY(EditAnywhere, Category = MySettings)
	TSubclassOf<AActor> chair_BP;

	class ABed* bedActor;

	class ARotationArrrowActor* RotationArrowActor;

private:
	AActor* spawnedIndicator;
	AActor* chair_inst;
	UUserWidget* widget_inst;

	void Horizontal(float value);
	void Vertical(float value);

	void Turn(float value);
	void LookUp(float value);

	void Zaxis(float value);

	void ray();
	
	void ShowPlaneOutLine();
	
	class AMapToolManager* mapToolManager;
	
	FVector GetTouchLocation(const FVector2D& touchPos);
	
	bool bCanSpawnActor = true;
	
protected:
   UPROPERTY(BlueprintReadOnly)
   AActor* ClickedActor = nullptr;

   FVector DragStartLocation; // 드래그 시작 위치
   FVector DragEndPosition;
   bool bIsDragging = false;

public:
   void OnLeftMouseButtonPressed();
   void OnLeftMouseButtonReleased();


   bool isMoveStart=false;
   bool isRotStart;
   bool isBedSpawn = true;

};

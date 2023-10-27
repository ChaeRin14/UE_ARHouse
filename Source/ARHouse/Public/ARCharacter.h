// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Engine/World.h"
#include "Engine/StaticMeshActor.h"
#include "DrawDebugHelpers.h"
#include "ARCharacter.generated.h"

UCLASS()
class ARHOUSE_API AARCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AARCharacter();

protected:
	virtual void BeginPlay() override;
	void PlaceIndicator(const FVector& Location);

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void UpdateIndicatorRotation();

	float speed = 500;
	FVector Dir;

	UPROPERTY(EditAnywhere, Category = MySettings)
	class UCapsuleComponent* BoxComp;
	
	UPROPERTY(EditAnywhere, Category = MySettings)
	class UStaticMeshComponent* BodyMesh;
	
	UPROPERTY(EditAnywhere, Category = MySettings)
	class USpringArmComponent* SpringArmComp;
	
	UPROPERTY(EditAnywhere, Category = MySettings)
	class UCameraComponent* CameraComponent;

	UPROPERTY(EditAnywhere, Category = MySettings)
	class APlayerController* pc;

	UPROPERTY(EditAnywhere, Category = MySettings)
	class UARSessionConfig* arSessionCinfig1;

	UPROPERTY(EditAnywhere, Category = MySettings)
	TSubclassOf<AActor> chair_BP;

	class ABed* bedActor;

	class ARotationArrrowActor* RotationArrowActor;

	bool bIsRotOnly = false;


private:
	AActor* spawnedIndicator;
	AActor* chair_inst;
	UUserWidget* widget_inst;

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

   void FBXImport();

   void LoadLevel();


   bool isMoveStart=false;
   bool isRotStart;
   bool isBedSpawn = true;



protected:
	UPROPERTY()
	AStaticMeshActor* IndicatorActor;

	public:

	UFUNCTION(BlueprintImplementableEvent)
	void OnHitLocationDetected(FVector HitLocation);

	UFUNCTION(BlueprintImplementableEvent)
	void OnHitActorDetected(AActor* HitActor, UPrimitiveComponent* HitComponent);

	UFUNCTION(BlueprintImplementableEvent)
	void OnHoverActorDetected(AActor* HoverActor, UPrimitiveComponent* HoveredComponent);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	UStaticMesh* IndicatorMarkerMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float FlySpeed;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float PanSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float TiltSpeed;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float MovementSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float MaximumMovementSpeed;

	UFUNCTION(BlueprintCallable, Category="Indicator")
	void DestroyIndicator();
	
	UFUNCTION(BlueprintCallable, Category="FlyCamCharacter")
	void MoveForward(float Value);

	UFUNCTION(BlueprintCallable, Category="FlyCamCharacter")
	void MoveRight(float Value);

	UFUNCTION(BlueprintCallable, Category="FlyCamCharacter")
	void Turn(float Value);

	UFUNCTION(BlueprintCallable, Category="FlyCamCharacter")
	void LookUp(float Value);

	UFUNCTION(BlueprintCallable, Category="FlyCamCharacter")
	void Tilt(float Value);

	
	void ChangeMovementSpeed(float Value);
	UWorld* GetGameWorld();
	bool CheckHitAtMouseCursor(FHitResult& objecthit);

	UFUNCTION(BlueprintCallable, Category="FlyCamCharacter")
	void DetectMouseHitLocation();

	UFUNCTION(BlueprintCallable, Category="FlyCamCharacter")
	void DetectMouseMoveAtLocation();

	class UMapToolWidget* maptoolWidget;
	
	UPROPERTY(BlueprintReadWrite)
	FString XLocationString;
};

// Copyright 2019-2023, Athian Games. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Engine/StaticMeshActor.h"
#include "FlycamCharacter.generated.h"

UCLASS()
class RUNTIMEFBXIMPORT_API AFlyCamCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AFlyCamCharacter();

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	class UCameraComponent* CameraComponent;


	void UpdateIndicatorRotation();
	

protected:
	
	void PlaceIndicator(const FVector& Location);
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

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
	
};
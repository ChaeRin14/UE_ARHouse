// Copyright 2019-2023, Athian Games. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SceneComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "PivotActor.generated.h"

UCLASS()
class RUNTIMEFBXIMPORT_API APivotActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APivotActor();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* MainScene;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* CenterMesh;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* ArrowX;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* ArrowY;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* ArrowZ;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* Handle2DXY;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* Handle2DXZ;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* Handle2DYZ;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pivot")
	float PivotSize;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pivot")
	float HandleScaleRatio;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pivot")
	float Sensitivity = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pivot")
	float InterpSpeed = 10.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision", meta = (AllowPrivateAccess = "true"))
	UCapsuleComponent* CollisionCapsuleX;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision", meta = (AllowPrivateAccess = "true"))
	UCapsuleComponent* CollisionCapsuleY;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision", meta = (AllowPrivateAccess = "true"))
	UCapsuleComponent* CollisionCapsuleZ;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* CollisionBoxXY;
		
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* CollisionBoxXZ;
		
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* CollisionBoxYZ;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision", meta = (AllowPrivateAccess = "true"))
	USphereComponent* CollisionSphereCenter;
	
	// Materials
	UPROPERTY()
	UMaterialInstanceDynamic* MaterialX;

	UPROPERTY()
	UMaterialInstanceDynamic* MaterialY;

	UPROPERTY()
	UMaterialInstanceDynamic* MaterialZ;

	UPROPERTY()
	APlayerController* PlayerController;

protected:

	UPROPERTY(Transient)
	bool bMousePressed;
	
	UPROPERTY()
	UPrimitiveComponent* CurrentTouchedComponent;

	void HandleMouseReleased();
	void HandleMouseClicked();
	void HandleMouseOverCollisionObjects();

	void SetCollisionProperties(UPrimitiveComponent* Component);

	bool bIsDraggingX = false;
	bool bIsDraggingY = false;
	bool bIsDraggingZ = false;
	FVector2D PreviousMousePos;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	

#if UE_EDITOR
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	FVector CalculateMovementVector(FVector ArrowDirection);
	
	virtual void NotifyActorBeginCursorOver() override;
	virtual void NotifyActorEndCursorOver() override;
	virtual void NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;
};

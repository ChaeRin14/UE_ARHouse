// Copyright 2019-2023, Athian Games. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SceneComponent.h"
#include "PivotActorBase.generated.h"

UCLASS()
class RUNTIMEFBXIMPORT_API APivotActorBase : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APivotActorBase();
	void SetCollisionProperties(UPrimitiveComponent* Component);
	void HandleMouseOverCollisionObjects();
	void HandleMouseClicked();
	void HandleMouseReleased();
	FVector CalculateMovementVector(FVector ArrowDirection);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* MainScene;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pivot")
	float PivotSize;

	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pivot")
	float Sensitivity = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pivot")
	float InterpSpeed = 10.0f;
	

protected:


	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()
	APlayerController* PlayerController;

	UPROPERTY()
	UPrimitiveComponent* CurrentTouchedComponent;

	UPROPERTY()
	bool bMousePressed;

	UPROPERTY()
	FVector2D PreviousMousePos;

	virtual void SetParameters()
	{
		bMousePressed = true;
	}

	virtual bool IsDragging()
	{
		return false;
	}
	
	virtual void ReSetParameters()
	{
		bMousePressed = false;
	}

	virtual FVector GetTotalMovementVector()
	{
		return FVector();
	}

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};

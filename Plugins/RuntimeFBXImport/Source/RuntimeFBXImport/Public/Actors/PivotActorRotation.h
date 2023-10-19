// Copyright 2019-2023, Athian Games. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "Actors/PivotActorBase.h"
#include "PivotActorRotation.generated.h"

/**
 * 
 */
UCLASS()
class RUNTIMEFBXIMPORT_API APivotActorRotation : public APivotActorBase
{
	GENERATED_BODY()

public:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* RotationMeshX;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* RotationMeshY;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* RotationMeshZ;

	APivotActorRotation();

};

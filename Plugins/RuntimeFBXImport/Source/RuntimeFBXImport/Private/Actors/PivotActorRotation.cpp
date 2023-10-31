// Copyright 2019-2023, Athian Games. All Rights Reserved. 


#include "Actors/PivotActorRotation.h"

APivotActorRotation::APivotActorRotation()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create arrow components
	RotationMeshX = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RotationMeshX"));
	RotationMeshX->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	RotationMeshY = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RotationMeshY"));
	RotationMeshY->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	RotationMeshZ = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RotationMeshZ"));
	RotationMeshZ->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	RotationMeshX->SetRelativeRotation(FRotator(0, 0, -90));
	RotationMeshY->SetRelativeRotation(FRotator(90, 0, 0));
	RotationMeshZ->SetRelativeRotation(FRotator(0, 0, 0));

	RotationMeshX->SetRelativeScale3D(FVector(PivotSize));
	RotationMeshY->SetRelativeScale3D(FVector(PivotSize));
	RotationMeshZ->SetRelativeScale3D(FVector(PivotSize));
}

// Copyright 2019-2023, Athian Games. All Rights Reserved. 


#include "Actors/PivotActorTranslation.h"


// Sets default values
APivotActorTranslation::APivotActorTranslation()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	HandleScaleRatio = 2;
	
	const float HandleScaleFactor = PivotSize / HandleScaleRatio;

	CenterMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CenterMesh"));
	CenterMesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	
	// Create arrow components
	ArrowX = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ArrowX"));
	ArrowX->AttachToComponent(CenterMesh, FAttachmentTransformRules::KeepRelativeTransform);

	ArrowY = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ArrowY"));
	ArrowY->AttachToComponent(CenterMesh, FAttachmentTransformRules::KeepRelativeTransform);

	ArrowZ = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ArrowZ"));
	ArrowZ->AttachToComponent(CenterMesh, FAttachmentTransformRules::KeepRelativeTransform);

	ArrowX->SetRelativeRotation(FRotator(0, 0, -90));
	ArrowY->SetRelativeRotation(FRotator(90, 0, 0));
	ArrowZ->SetRelativeRotation(FRotator(0, 0, 0));


	
	Handle2DXY = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Handle2DXY"));
	Handle2DXY->AttachToComponent(ArrowZ, FAttachmentTransformRules::KeepRelativeTransform);

	Handle2DXZ = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Handle2DXZ"));
	Handle2DXZ->AttachToComponent(ArrowY, FAttachmentTransformRules::KeepRelativeTransform);

	Handle2DYZ = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Handle2DYZ"));
	Handle2DYZ->AttachToComponent(ArrowX, FAttachmentTransformRules::KeepRelativeTransform);

	UpdateHandles();

	// Collision Capsule for ArrowX
	CollisionCapsuleX = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CollisionCapsuleX"));
	CollisionCapsuleX->SetCapsuleSize(50.0f, 200.0f); // You can adjust these values
	CollisionCapsuleX->SetupAttachment(ArrowX);

	// Collision Capsule for ArrowY
	CollisionCapsuleY = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CollisionCapsuleY"));
	CollisionCapsuleY->SetCapsuleSize(50.0f, 200.0f); // You can adjust these values
	CollisionCapsuleY->SetupAttachment(ArrowY);

	// Collision Capsule for ArrowZ
	CollisionCapsuleZ = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CollisionCapsuleZ"));
	CollisionCapsuleZ->SetCapsuleSize(50.0f, 200.0f); // You can adjust these values
	CollisionCapsuleZ->SetupAttachment(ArrowZ);

	// Collision Box for Handle2DXY
	CollisionBoxXY = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBoxXY"));
	CollisionBoxXY->SetRelativeLocation(HandleScaleFactor * FVector(100, -100, 0));
	CollisionBoxXY->SetBoxExtent(FVector(50.f, 50.f, 10.f)); // Modify this to the required size
	CollisionBoxXY->SetupAttachment(Handle2DXY);

	// Collision Box for Handle2DXZ
	CollisionBoxXZ = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBoxXZ"));
	CollisionBoxXZ->SetRelativeLocation(HandleScaleFactor * FVector(100, -100, 0));
	CollisionBoxXZ->SetBoxExtent(FVector(50.f, 50.f, 10.f)); // Modify this to the required size
	CollisionBoxXZ->SetupAttachment(Handle2DXZ);

	// Collision Box for Handle2DYZ
	CollisionBoxYZ = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBoxYZ"));
	CollisionBoxYZ->SetRelativeLocation(HandleScaleFactor * FVector(100, -100, 0));
	CollisionBoxYZ->SetBoxExtent(FVector(50.f, 50.f, 10.f)); // Modify this to the required size
	CollisionBoxYZ->SetupAttachment(Handle2DYZ);
	
	CollisionSphereCenter = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphereCenter"));
	CollisionSphereCenter->SetSphereRadius(10.f); // Modify this to the required radius
	CollisionSphereCenter->SetupAttachment(CenterMesh);
	
	ArrowX->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ArrowY->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ArrowZ->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Handle2DXY->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Handle2DXZ->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Handle2DYZ->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CenterMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called when the game starts or when spawned
void APivotActorTranslation::BeginPlay()
{
	Super::BeginPlay();

	SetCollisionProperties(CollisionCapsuleX);
	SetCollisionProperties(CollisionCapsuleY);
	SetCollisionProperties(CollisionCapsuleZ);
	SetCollisionProperties(CollisionBoxXY);
	SetCollisionProperties(CollisionBoxXZ);
	SetCollisionProperties(CollisionBoxYZ);
	SetCollisionProperties(CollisionSphereCenter);
}

void APivotActorTranslation::SetParameters()
{
	Super::SetParameters();
	
	if (CurrentTouchedComponent == CollisionCapsuleX)
	{
		bIsDraggingX = true;
	}
	else if (CurrentTouchedComponent == CollisionCapsuleY)
	{
		bIsDraggingY = true;
	}
	else if (CurrentTouchedComponent == CollisionCapsuleZ)
	{
		bIsDraggingZ = true;
	}
	else if (CurrentTouchedComponent == CollisionBoxXY)
	{
		bIsDraggingX = true;
		bIsDraggingY = true;
	}
	else if (CurrentTouchedComponent == CollisionBoxXZ)
	{
		bIsDraggingY = true;
		bIsDraggingZ = true;
	}
	else if (CurrentTouchedComponent == CollisionBoxYZ)
	{
		bIsDraggingX = true;
		bIsDraggingZ = true;
	}
	else if (CurrentTouchedComponent == CollisionSphereCenter)
	{
		bIsDraggingX = true;
		bIsDraggingY = true;
		bIsDraggingZ = true;
	}

	
		
}

void APivotActorTranslation::ReSetParameters()
{
	Super::ReSetParameters();

	bIsDraggingX = false;
	bIsDraggingY = false;
	bIsDraggingZ = false;

}

FVector APivotActorTranslation::GetTotalMovementVector()
{
	FVector AccumulatedMovementVector = FVector::ZeroVector;
			
	if (bIsDraggingX)
	{
		FVector ArrowDirection = -CollisionCapsuleX->GetUpVector();
		AccumulatedMovementVector += CalculateMovementVector(ArrowDirection);
	}

	if (bIsDraggingY)
	{
		FVector ArrowDirection = CollisionCapsuleY->GetUpVector();
		AccumulatedMovementVector += CalculateMovementVector(ArrowDirection);
	}

	if (bIsDraggingZ)
	{
		FVector ArrowDirection = CollisionCapsuleZ->GetUpVector();
		AccumulatedMovementVector += CalculateMovementVector(ArrowDirection);
	}

	return AccumulatedMovementVector;
}

void APivotActorTranslation::UpdateHandles()
{
	ArrowZ->SetRelativeScale3D(FVector(PivotSize));
	ArrowY->SetRelativeScale3D(FVector(PivotSize));
	ArrowX->SetRelativeScale3D(FVector(PivotSize));
	
	const float HandleScaleFactor = PivotSize / HandleScaleRatio;

	Handle2DXY->SetRelativeLocation(HandleScaleFactor * FVector(-100, -100, 0));
	Handle2DXZ->SetRelativeLocation(HandleScaleFactor * FVector(100, -100, 0));
	Handle2DYZ->SetRelativeLocation(HandleScaleFactor * FVector(-100, 100, 0));
			
	Handle2DXY->SetRelativeRotation(FRotator(0, 0, 180));
	Handle2DXZ->SetRelativeRotation(FRotator(0, 180, 0));
	Handle2DYZ->SetRelativeRotation(FRotator(0, 0, 0));

	Handle2DXY->SetRelativeScale3D(FVector(HandleScaleFactor));
	Handle2DXZ->SetRelativeScale3D(FVector(HandleScaleFactor));
	Handle2DYZ->SetRelativeScale3D(FVector(HandleScaleFactor));

}

// Called every frame
void APivotActorTranslation::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

#if UE_EDITOR

void APivotActorTranslation::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void APivotActorTranslation::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	// Here we check the name of the property that was changed
	if(PropertyChangedEvent.Property != nullptr)
	{
		FName PropertyName = PropertyChangedEvent.GetPropertyName();

		// If the PivotSize property was changed
		if(PropertyName == GET_MEMBER_NAME_CHECKED(APivotActorTranslation, PivotSize) ||
			PropertyName == GET_MEMBER_NAME_CHECKED(APivotActorTranslation, HandleScaleRatio))
		{
			UpdateHandles();
		}
	}
}
#endif
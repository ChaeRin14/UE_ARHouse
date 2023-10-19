// Copyright 2019-2023, Athian Games. All Rights Reserved. 


#include "Actors/PivotActor.h"


#include "Kismet/GameplayStatics.h"

// Sets default values
APivotActor::APivotActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PivotSize = 0.5f;
	HandleScaleRatio = 2;


	MainScene = CreateDefaultSubobject<USceneComponent>(TEXT("MainScene"));
	RootComponent = MainScene;

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

	Handle2DXY = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Handle2DXY"));
	Handle2DXY->AttachToComponent(ArrowZ, FAttachmentTransformRules::KeepRelativeTransform);

	Handle2DXZ = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Handle2DXZ"));
	Handle2DXZ->AttachToComponent(ArrowY, FAttachmentTransformRules::KeepRelativeTransform);

	Handle2DYZ = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Handle2DYZ"));
	Handle2DYZ->AttachToComponent(ArrowX, FAttachmentTransformRules::KeepRelativeTransform);

	ArrowX->SetRelativeRotation(FRotator(0, 0, -90));
	ArrowX->SetRelativeScale3D(FVector(PivotSize));
	
	ArrowY->SetRelativeRotation(FRotator(90, 0, 0));
	ArrowY->SetRelativeScale3D(FVector(PivotSize));
	
	ArrowZ->SetRelativeRotation(FRotator(0, 0, 0));
	ArrowZ->SetRelativeScale3D(FVector(PivotSize));

	Handle2DXZ->SetRelativeLocation(HandleScaleFactor * FVector(100, -100, 0));
	Handle2DXZ->SetRelativeRotation(FRotator(180, 0, 90));
	Handle2DXZ->SetRelativeScale3D(FVector(HandleScaleFactor));
	
	Handle2DXY->SetRelativeLocation(HandleScaleFactor * FVector(-100, -100, 0));
	Handle2DXY->SetRelativeRotation(FRotator(180, 0, 0));
	Handle2DXY->SetRelativeScale3D(FVector(HandleScaleFactor));

	Handle2DYZ->SetRelativeLocation(HandleScaleFactor * FVector(-100, 100, 0));
	Handle2DYZ->SetRelativeRotation(FRotator(0, 0, 0));
	Handle2DYZ->SetRelativeScale3D(FVector(HandleScaleFactor));

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

void APivotActor::SetCollisionProperties(UPrimitiveComponent* Component)
{
	if (Component)
	{
		
		Component->SetCollisionObjectType(ECC_Visibility);
		Component->SetCollisionResponseToAllChannels(ECR_Overlap);
		Component->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
		Component->SetGenerateOverlapEvents(true);

		// Enable the collision
		Component->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
}

void APivotActor::HandleMouseOverCollisionObjects()
{
	CurrentTouchedComponent = nullptr;

	if (PlayerController)
	{
		TArray<FHitResult> HitResults;
		if (!bMousePressed)
		{
			PlayerController->CurrentMouseCursor = EMouseCursor::Default;
		}

		FVector MouseWorldLocation, MouseWorldDirection;
		PlayerController->DeprojectMousePositionToWorld(MouseWorldLocation, MouseWorldDirection);

		FVector StartLocation = MouseWorldLocation;
		FVector EndLocation = StartLocation + (MouseWorldDirection * 10000.0f); 

		FCollisionQueryParams TraceParams(FName(TEXT("VisibilityTrace")), true);
		// LineTraceMulti for multiple hits
		PlayerController->GetWorld()->LineTraceMultiByChannel(HitResults, StartLocation, EndLocation, ECC_Visibility, TraceParams);
			
		for (const FHitResult& Hit : HitResults)
		{
			if (Hit.Component == CollisionCapsuleX ||
				Hit.Component == CollisionCapsuleY ||
				Hit.Component == CollisionCapsuleZ ||
				Hit.Component == CollisionBoxXY ||
				Hit.Component == CollisionBoxXZ ||
				Hit.Component == CollisionBoxYZ ||
				Hit.Component == CollisionSphereCenter)
			{
				if (!bMousePressed)
				{
					PlayerController->CurrentMouseCursor = EMouseCursor::CardinalCross;
				}

				CurrentTouchedComponent = Hit.Component.Get();
				break;
			}
		}
	}

}


void APivotActor::HandleMouseReleased()
{
	if(PlayerController)
	{
		FInputModeGameAndUI InputModeGameAndUI;
		InputModeGameAndUI.SetLockMouseToViewportBehavior(EMouseLockMode::LockOnCapture);
		PlayerController->SetInputMode(InputModeGameAndUI);

		bIsDraggingX = false;
		bIsDraggingY = false;
		bIsDraggingZ = false;
		bMousePressed = false;

		PlayerController->CurrentMouseCursor = EMouseCursor::Default;
		HandleMouseOverCollisionObjects();
	}
}

void APivotActor::HandleMouseClicked()
{
	if (CurrentTouchedComponent == CollisionCapsuleX)
	{
		bIsDraggingX = true;
		bMousePressed = true;
	}
	else if (CurrentTouchedComponent == CollisionCapsuleY)
	{
		bIsDraggingY = true;
		bMousePressed = true;
	}
	else if (CurrentTouchedComponent == CollisionCapsuleZ)
	{
		bIsDraggingZ = true;
		bMousePressed = true;
	}
	else if (CurrentTouchedComponent == CollisionBoxXY)
	{
		bIsDraggingX = true;
		bIsDraggingY = true;
		bMousePressed = true;
	}
	else if (CurrentTouchedComponent == CollisionBoxXZ)
	{
		bIsDraggingY = true;
		bIsDraggingZ = true;
		bMousePressed = true;
	}
	else if (CurrentTouchedComponent == CollisionBoxYZ)
	{
		bIsDraggingX = true;
		bIsDraggingZ = true;
		bMousePressed = true;
	}
	else if (CurrentTouchedComponent == CollisionSphereCenter)
	{
		bIsDraggingX = true;
		bIsDraggingY = true;
		bIsDraggingZ = true;
		bMousePressed = true;
	}
		
	if(PlayerController)
	{
		if(bIsDraggingX || bIsDraggingY || bIsDraggingZ)
		{
			PlayerController->CurrentMouseCursor = EMouseCursor::GrabHandClosed;
		
			PlayerController->GetMousePosition(PreviousMousePos.X, PreviousMousePos.Y);
			FInputModeGameAndUI InputModeGameAndUI;
		
			InputModeGameAndUI.SetLockMouseToViewportBehavior(EMouseLockMode::LockOnCapture);
			InputModeGameAndUI.SetHideCursorDuringCapture(false);
			PlayerController->SetInputMode(InputModeGameAndUI);
		}
	}
}


// Called when the game starts or when spawned
void APivotActor::BeginPlay()
{
	Super::BeginPlay();
	bMousePressed = false;

	// Create dynamic material instances
	MaterialX = UMaterialInstanceDynamic::Create(ArrowX->GetMaterial(0), this);
	MaterialY = UMaterialInstanceDynamic::Create(ArrowY->GetMaterial(0), this);
	MaterialZ = UMaterialInstanceDynamic::Create(ArrowZ->GetMaterial(0), this);

	ArrowX->SetMaterial(0, MaterialX);
	ArrowY->SetMaterial(0, MaterialY);
	ArrowZ->SetMaterial(0, MaterialZ);

	// Get the player controller
	PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);;
	PlayerController->bEnableClickEvents = true;
	PlayerController->bEnableMouseOverEvents = true;
	
	SetCollisionProperties(CollisionCapsuleX);
	SetCollisionProperties(CollisionCapsuleY);
	SetCollisionProperties(CollisionCapsuleZ);
	SetCollisionProperties(CollisionBoxXY);
	SetCollisionProperties(CollisionBoxXZ);
	SetCollisionProperties(CollisionBoxYZ);
	SetCollisionProperties(CollisionSphereCenter);
}

// Called every frame
void APivotActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Get the player's camera
	APlayerCameraManager* PlayerCameraManager = UGameplayStatics::GetPlayerCameraManager(this, 0);
	if (!PlayerCameraManager) return;

	// Calculate distance from the camera
	float Distance = FVector::Dist(PlayerCameraManager->GetCameraLocation(), GetActorLocation());

	float Scale = Distance * PivotSize / 1000;
	SetActorScale3D(FVector(Scale, Scale, Scale));

	HandleMouseOverCollisionObjects();
	
	if (PlayerController)
	{
		
		if (bMousePressed && !PlayerController->IsInputKeyDown(EKeys::LeftMouseButton))
		{
			HandleMouseReleased();
		}
		// Update the mouse pressed state
		else if (!bMousePressed  && PlayerController->IsInputKeyDown(EKeys::LeftMouseButton))
		{
			HandleMouseClicked();
		}

		if(bMousePressed)
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

			if (!AccumulatedMovementVector.IsNearlyZero())
			{
				FVector DesiredLocation = GetActorLocation() + AccumulatedMovementVector;
				FVector NewLocation = FMath::VInterpTo(GetActorLocation(), DesiredLocation, GetWorld()->GetDeltaSeconds(), InterpSpeed);
				SetActorLocation(NewLocation);

				
			}
		}
	
	}
	
}

FVector APivotActor::CalculateMovementVector(FVector ArrowDirection)
{
    // Calculate a point and normal for the movement plane.
    FVector PlanePoint = GetActorLocation();
    FVector PlaneNormal;

    if (ArrowDirection.Equals(FVector::UpVector)) 
    {
        // For upward movement, the plane should be perpendicular to camera's forward direction.
        PlaneNormal = FVector::CrossProduct(FVector::UpVector, PlayerController->GetControlRotation().Vector()).GetSafeNormal();
    } 
    else 
    {
        // For sideways movement, use a vertical plane.
        PlaneNormal = FVector::CrossProduct(ArrowDirection, FVector::UpVector).GetSafeNormal();
    }

    // Project current and previous mouse positions onto the plane.
    FVector CurrentMouseWorldLocation, CurrentMouseWorldDirection;
    FVector PreviousMouseWorldLocation, PreviousMouseWorldDirection;

    FVector2D CurrentMousePos;
    PlayerController->GetMousePosition(CurrentMousePos.X, CurrentMousePos.Y);
    
    PlayerController->DeprojectScreenPositionToWorld(CurrentMousePos.X, CurrentMousePos.Y, CurrentMouseWorldLocation, CurrentMouseWorldDirection);
    PlayerController->DeprojectScreenPositionToWorld(PreviousMousePos.X, PreviousMousePos.Y, PreviousMouseWorldLocation, PreviousMouseWorldDirection);

    FVector CurrentMouseHitPoint = FMath::LinePlaneIntersection(CurrentMouseWorldLocation,
                                                            CurrentMouseWorldLocation + CurrentMouseWorldDirection *
                                                            10000, PlanePoint, PlaneNormal);
    FVector PreviousMouseHitPoint = FMath::LinePlaneIntersection(PreviousMouseWorldLocation,
                                                             PreviousMouseWorldLocation +
                                                             PreviousMouseWorldDirection * 10000, PlanePoint,
                                                             PlaneNormal);

    // Calculate the movement direction and magnitude.
    FVector MovementVector = (CurrentMouseHitPoint - PreviousMouseHitPoint) * Sensitivity;

    // Restrict movement to the desired direction.
    MovementVector = FVector::DotProduct(MovementVector, ArrowDirection) * ArrowDirection;
	PreviousMousePos = CurrentMousePos;
	
    return MovementVector;
}

void APivotActor::NotifyActorBeginCursorOver()
{
	// Change color when hovered
	if (MaterialX) MaterialX->SetVectorParameterValue("BaseColor", FLinearColor::Red);
	if (MaterialY) MaterialY->SetVectorParameterValue("BaseColor", FLinearColor::Green);
	if (MaterialZ) MaterialZ->SetVectorParameterValue("BaseColor", FLinearColor::Blue);
}

void APivotActor::NotifyActorEndCursorOver()
{
	// Reset color when not hovered
	if (MaterialX) MaterialX->SetVectorParameterValue("BaseColor", FLinearColor::White);
	if (MaterialY) MaterialY->SetVectorParameterValue("BaseColor", FLinearColor::White);
	if (MaterialZ) MaterialZ->SetVectorParameterValue("BaseColor", FLinearColor::White);
}

void APivotActor::NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	// Change color when dragging
	if (MaterialX) MaterialX->SetVectorParameterValue("BaseColor", FLinearColor::Yellow);
	if (MaterialY) MaterialY->SetVectorParameterValue("BaseColor", FLinearColor::Yellow);
	if (MaterialZ) MaterialZ->SetVectorParameterValue("BaseColor", FLinearColor::Yellow);
}

#if UE_EDITOR

void APivotActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void APivotActor::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	// Here we check the name of the property that was changed
	if(PropertyChangedEvent.Property != nullptr)
	{
		FName PropertyName = PropertyChangedEvent.GetPropertyName();

		// If the PivotSize property was changed
		if(PropertyName == GET_MEMBER_NAME_CHECKED(APivotActor, PivotSize))
		{
			ArrowX->SetRelativeScale3D(FVector(PivotSize));
			ArrowY->SetRelativeScale3D(FVector(PivotSize));
			ArrowZ->SetRelativeScale3D(FVector(PivotSize));
		}

		// If the HandleScaleRatio property was changed
		else if(PropertyName == GET_MEMBER_NAME_CHECKED(APivotActor, HandleScaleRatio))
		{
			const float HandleScaleFactor = PivotSize / HandleScaleRatio;

			Handle2DXY->SetRelativeScale3D(FVector(HandleScaleFactor));
			Handle2DXZ->SetRelativeScale3D(FVector(HandleScaleFactor));
			Handle2DYZ->SetRelativeScale3D(FVector(HandleScaleFactor));

			Handle2DXY->SetRelativeLocation(HandleScaleFactor * FVector(100, -100, 0));
			Handle2DXZ->SetRelativeLocation(HandleScaleFactor * FVector(-100, -100, 0));
			Handle2DYZ->SetRelativeLocation(HandleScaleFactor * FVector(-100, 100, 0));
		}
	}
}
#endif

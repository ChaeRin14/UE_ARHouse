// Copyright 2019-2023, Athian Games. All Rights Reserved. 

#include "Actors/PivotActorBase.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
APivotActorBase::APivotActorBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	PivotSize = 0.5f;

	MainScene = CreateDefaultSubobject<USceneComponent>(TEXT("MainScene"));
	RootComponent = MainScene;
}

// Called when the game starts or when spawned
void APivotActorBase::BeginPlay()
{
	Super::BeginPlay();

	// Get the player controller
	PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);;
	PlayerController->bEnableClickEvents = true;
	PlayerController->bEnableMouseOverEvents = true;

}

// Called every frame
void APivotActorBase::Tick(float DeltaTime)
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
			FVector AccumulatedMovementVector = GetTotalMovementVector();
			if (!AccumulatedMovementVector.IsNearlyZero())
			{
				FVector DesiredLocation = GetActorLocation() + AccumulatedMovementVector;
				FVector NewLocation = FMath::VInterpTo(GetActorLocation(), DesiredLocation, GetWorld()->GetDeltaSeconds(), InterpSpeed);
				SetActorLocation(NewLocation);
			}
		}
	
	}
}

void APivotActorBase::SetCollisionProperties(UPrimitiveComponent* Component)
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

void APivotActorBase::HandleMouseOverCollisionObjects()
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
			if (Hit.Component->GetOwner() == this)
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

void APivotActorBase::HandleMouseClicked()
{
	if(CurrentTouchedComponent)
	{
		SetParameters();
	}
	
	if(PlayerController)
	{
		if(IsDragging())
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

void APivotActorBase::HandleMouseReleased()
{
	if(PlayerController)
	{
		FInputModeGameAndUI InputModeGameAndUI;
		InputModeGameAndUI.SetLockMouseToViewportBehavior(EMouseLockMode::LockOnCapture);
		PlayerController->SetInputMode(InputModeGameAndUI);

	
		ReSetParameters();
		

		PlayerController->CurrentMouseCursor = EMouseCursor::Default;
		HandleMouseOverCollisionObjects();
	}
}

FVector APivotActorBase::CalculateMovementVector(FVector ArrowDirection)
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



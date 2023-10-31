// Copyright 2019-2023, Athian Games. All Rights Reserved. 

#include "FlyCamCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AFlyCamCharacter::AFlyCamCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(RootComponent);

	FlySpeed = 10;
	PanSpeed = 10;
	TiltSpeed = 5;
	MovementSpeed = 0.4;
	MaximumMovementSpeed = 100;

	
}

// Called when the game starts or when spawned
void AFlyCamCharacter::BeginPlay()
{
	
	Super::BeginPlay();

	// Calculate the initial point for the line trace
	FVector InitialPoint = CameraComponent->GetComponentLocation() + CameraComponent->GetForwardVector() * 2000;

	// Calculate the start and end points for the vertical line trace
	FVector StartLocation = InitialPoint;
	FVector EndLocation = FVector(InitialPoint.X, InitialPoint.Y, InitialPoint.Z - 50000);

	FCollisionQueryParams TraceParams(FName(TEXT("GroundTrace")), true, this);
	TraceParams.bTraceComplex = true;
	TraceParams.bReturnPhysicalMaterial = false;
	FHitResult HitResult;

	// Perform the line trace
	if (GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, TraceParams))
	{
		// Place the indicator marker at the hit location
		PlaceIndicator(HitResult.Location);
		OnHitLocationDetected(HitResult.Location);
	}
}

// Called every frame
void AFlyCamCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateIndicatorRotation();
	DetectMouseMoveAtLocation();
}

// Called to bind functionality to input
void AFlyCamCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);



}

void AFlyCamCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		FVector CameraLocation = CameraComponent->GetComponentLocation();
		FVector TraceEndLocation = CameraLocation + UKismetMathLibrary::GetForwardVector(CameraComponent->GetRelativeRotation()) * 5000;
		FRotator ForwardRotation = UKismetMathLibrary::FindLookAtRotation(CameraLocation, TraceEndLocation);
		FVector Direction = UKismetMathLibrary::GetForwardVector(ForwardRotation);
		AddActorLocalOffset(Direction * Value * FlySpeed);
	}
}

void AFlyCamCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		FVector CameraLocation = CameraComponent->GetComponentLocation();
		FVector TraceEndLocation = CameraLocation + UKismetMathLibrary::GetRightVector(CameraComponent->GetRelativeRotation()) * 5000;
		FRotator ForwardRotation = UKismetMathLibrary::FindLookAtRotation(CameraLocation, TraceEndLocation);
		FVector Direction = UKismetMathLibrary::GetForwardVector(ForwardRotation);
		AddActorLocalOffset(Direction * Value * FlySpeed);
	}
}

void AFlyCamCharacter::Turn(float Value)
{
	CameraComponent->AddRelativeRotation(FRotator(0, Value, 0));
	
}

void AFlyCamCharacter::LookUp(float Value)
{
	FRotator CameraRotation =  CameraComponent->GetRelativeRotation();
	CameraRotation.Pitch = FMath::Clamp(CameraRotation.Pitch + Value, -80, 80);
	CameraComponent->SetRelativeRotation(CameraRotation);
}

void AFlyCamCharacter::Tilt(float Value)
{
	if (Value != 0.0f)
	{
		FVector TiltMovement =  FVector(0, 0 , Value * TiltSpeed);
		AddActorLocalOffset(TiltMovement);
	}
}

void AFlyCamCharacter::ChangeMovementSpeed(float Value)
{
	MovementSpeed = MovementSpeed + Value * 0.01;
	MovementSpeed = FMath::Clamp(MovementSpeed, 0.01, MaximumMovementSpeed);
}

UWorld* AFlyCamCharacter::GetGameWorld()
{
	if (UGameViewportClient* Viewport = GEngine->GameViewport)
	{
		FWorldContext* worldcontext = GEngine->GetWorldContextFromGameViewport(Viewport);
		UWorld* world = worldcontext->World();
		return world;
	}

	return nullptr;
}


bool AFlyCamCharacter::CheckHitAtMouseCursor(FHitResult &objecthit)
{
	if (UWorld* world = GetGameWorld())
	{

		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Visibility));
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));

		if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(world, 0))
		{
			PlayerController->HitResultTraceDistance = 10000000;
			bool isactorhit = PlayerController->GetHitResultUnderCursorForObjects(ObjectTypes, true, objecthit);
			return isactorhit;
		}
	}
	return false;

}


void AFlyCamCharacter::DetectMouseHitLocation()
{
	FHitResult HitResult;
	if (CheckHitAtMouseCursor(HitResult))
	{
		if(AActor* HitActor = HitResult.GetActor())
		{
			if(HitActor->IsA(AStaticMeshActor::StaticClass()))
			{
				PlaceIndicator(HitResult.Location);
				OnHitLocationDetected(HitResult.Location);
			}
			else
			{
				OnHitActorDetected(HitActor, HitResult.GetComponent());
			}
		}
		
	}
	
}

void AFlyCamCharacter::DetectMouseMoveAtLocation()
{
	FHitResult HitResult;
	if (CheckHitAtMouseCursor(HitResult))
	{
		if(AActor* HitActor = HitResult.GetActor())
		{
			OnHoverActorDetected(HitActor, HitResult.GetComponent());
		}
		
	}
}

void AFlyCamCharacter::UpdateIndicatorRotation()
{
	if(IndicatorActor)
	{
		FVector CameraLocation = CameraComponent->GetComponentLocation();
		FVector IndicatorLocation = IndicatorActor->GetActorLocation();
		
		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(CameraLocation, IndicatorLocation);
		LookAtRotation.Yaw -= 90;
		IndicatorActor->SetActorRotation(FRotator(0, LookAtRotation.Yaw, 0));
	}
}

void AFlyCamCharacter::DestroyIndicator()
{
	if(IndicatorActor)
	{
		IndicatorActor->Destroy();
		IndicatorActor = nullptr;
	}
}

void AFlyCamCharacter::PlaceIndicator(const FVector& Location)
{
	// Get a reference to the world
	if (UWorld* World = GetGameWorld())
	{
		if(IndicatorMarkerMesh)
		{
			DestroyIndicator();

			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
			AActor* IActor = World->SpawnActor<AStaticMeshActor>(Location, FRotator::ZeroRotator, SpawnParams);
			IndicatorActor = Cast<AStaticMeshActor>(IActor);
			IndicatorActor->SetMobility(EComponentMobility::Movable);

			// Set the static mesh for the spawned actor
			if (UStaticMeshComponent* StaticMeshComponent = IndicatorActor->GetStaticMeshComponent())
			{
				StaticMeshComponent->SetStaticMesh(IndicatorMarkerMesh);
				FBoxSphereBounds Bounds = StaticMeshComponent->Bounds;
				FVector NewLocation = Location;
				NewLocation.Z += Bounds.BoxExtent.Z + 10;
				IndicatorActor->SetActorLocation(NewLocation);

				// Update the indicator rotation to face the camera
				UpdateIndicatorRotation();
			}
		}
	}
}

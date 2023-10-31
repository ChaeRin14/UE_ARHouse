// Fill out your copyright notice in the Description page of Project Settings.


#include "ARCharacter.h"
#include "Components/CapsuleComponent.h"
#include <GameFramework/Pawn.h>
#include <Camera/CameraComponent.h>
#include <GameFramework/SpringArmComponent.h>
#include "ARBlueprintLibrary.h"
#include <Kismet/GameplayStatics.h>
#include "MapToolManager.h"
#include "MapToolWidget.h"
#include "RotationArrrowActor.h"
#include "Math/Axis.h"
#include <Kismet/KismetMathLibrary.h>

// Sets default values
AARCharacter::AARCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(RootComponent);
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("arCamComp"));
	CameraComponent->SetupAttachment(SpringArmComp);

	AutoPossessPlayer = EAutoReceiveInput::Player0;

	FlySpeed = 20;
	PanSpeed = 10;
	TiltSpeed = 10;
	MovementSpeed = 1;
	MaximumMovementSpeed = 100;

}

// Called when the game starts or when spawned
void AARCharacter::BeginPlay()
{
	Super::BeginPlay();

	pc = GetController<APlayerController>();

	// AR ī�޶� �ѱ�
	UARBlueprintLibrary::SetEnabledXRCamera(true);

	// ������ ������ AR ���� ����
	UARBlueprintLibrary::StartARSession(arSessionCinfig1);

	// �ڵ����� ���� ��带 ����
	UKismetSystemLibrary::ControlScreensaver(false);

	FVector InitialPoint = CameraComponent->GetComponentLocation() + CameraComponent->GetForwardVector() * 2000;

	// Calculate the start and end points for the vertical line trace
	FVector StartLocation = InitialPoint;
	FVector EndLocation = FVector(InitialPoint.X, InitialPoint.Y, InitialPoint.Z - 100000);

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
void AARCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector p0 = GetActorLocation();
	FVector vt = Dir * speed * DeltaTime;
	FVector p = p0 + vt;
	SetActorLocation(p, true);

	FVector2D firstTouch;
	bool bIsFirstTouch = true;

	pc->GetInputTouchState(ETouchIndex::Touch1, firstTouch.X, firstTouch.Y, bIsFirstTouch);

	if (chair_inst == nullptr && bIsFirstTouch)
	{
	 
		if (bisFirstSpawn)
		{
			FActorSpawnParameters param;
			param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			GetWorld()->SpawnActor<AActor>(chair_BP, GetTouchLocation(firstTouch), FRotator::ZeroRotator, param);
			UE_LOG(LogTemp, Warning, TEXT("bisFirstSpawn: %s"), bisFirstSpawn ? TEXT("true") : TEXT("false"));
			bisFirstSpawn = false;
		}
		
	
		
	}


	// YourActor�� ������ �������̸�, �̸� ����Ͽ� X ��ġ�� �����ɴϴ�.
	if (bedActor != nullptr)
	{
		// ������ ���� ��ġ�� �����ɴϴ�.
		FVector ActorLocation = bedActor->GetActorLocation();

		// X ��ġ�� ���ڿ��� ��ȯ�մϴ�.
		XLocationString = FString::Printf(TEXT("X: %.2f"), ActorLocation.X);

		//txt_X->SetText(FText::FromString(LocationText));

	}


	// ħ�� �̵�
	if (bIsDragging && ClickedActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("dqwdqwdeqf"));
		if (bIsRotOnly)
		{
			UE_LOG(LogTemp, Warning, TEXT("bIsRotOnly"));
			// ���� ���콺 ��ġ�� ���� ���콺 ��ġ ������ X ��ȭ���� ����մϴ�.
			float DeltaX = DragEndPosition.X - DragStartLocation.X;

			// ȸ�� �ӵ� ������ ���� ���� ����
			float RotationSpeed = 1.0f;

			// �����Ӵ� ȸ���� ���
			float RotationAmount = -DeltaX * RotationSpeed * GetWorld()->GetDeltaSeconds();  // ������ ����

			// ������ ���� ȸ����
			FRotator CurrentRotation = ClickedActor->GetActorRotation();

			// Yaw ȸ������ ����Ͽ� ������Ʈ�մϴ�.
			float NewYaw = CurrentRotation.Yaw + RotationAmount;

			// Yaw ���� 0~360 ������ �����մϴ�.
			NewYaw = FMath::Fmod(NewYaw, 360.0f);

			// ���� ���� ��쿡�� ����� ��ȯ�մϴ�.
			if (NewYaw < 0)
			{
				NewYaw += 360.0f;
			}

			// ���ο� ȸ���� ����
			FRotator NewRotation = FRotator(CurrentRotation.Pitch, NewYaw, CurrentRotation.Roll);

			// ������ ȸ�� ������Ʈ�ϱ�
			//ClickedActor->SetActorRotation(NewRotation);

			RotationArrowActor->SetActorRotation(NewRotation);
		}

		else
		{
			FVector WorldOrigin;
			FVector WorldDirection;

			// ���� ���콺 ��ġ ���� ��ǥ�� ��ȯ�ϱ�
			UGameplayStatics::GetPlayerController(this, 0)->DeprojectMousePositionToWorld(WorldOrigin, WorldDirection);

			// ���ο� ��ġ ����ϱ�
			DragEndPosition = WorldOrigin + (WorldDirection * 1000.f); // �̵� �Ÿ� ���� ����

			// ������ ��ġ ������Ʈ�ϱ�
			FVector DragDelta = DragEndPosition - DragStartLocation;
			DragDelta.Z = 0;
			ClickedActor->SetActorLocation(DragStartLocation + DragDelta);
			bIsRotOnly = false;
		}

	}


	// ȸ�� ��ư�� ������ ȸ�� ������Ʈ�� �����ǰ�
	if (isRotStart && !RotationArrowActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("sdquwdhk"));
		bedActor = Cast<ABed>(UGameplayStatics::GetActorOfClass(GetWorld(), ABed::StaticClass()));

		if (bedActor)
		{
			FVector SpawnLocation = bedActor->GetActorLocation() + FVector(0, 0, 70);; // ���� ��ġ ���� (bedActor�� ��ġ�� ����)
			FRotator SpawnRotation = FRotator::ZeroRotator; // ���� ȸ�� ����

			RotationArrowActor = GetWorld()->SpawnActor<ARotationArrrowActor>(SpawnLocation, SpawnRotation);
			bedActor->SetActorLocation(FVector(0, 0, -60));


			if (RotationArrowActor && bedActor)
			{
				bedActor->AttachToComponent(RotationArrowActor->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
				isBedSpawn = false;

			}
		}

	}


	UpdateIndicatorRotation();
	DetectMouseMoveAtLocation();

}

// Called to bind functionality to input
void AARCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);



	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &AARCharacter::Turn);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &AARCharacter::LookUp);
	PlayerInputComponent->BindAxis(TEXT("Forward"), this, &AARCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("Right"), this, &AARCharacter::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("Tilt"), this, &AARCharacter::Tilt);
	PlayerInputComponent->BindAction("MouseLeftButton", IE_Pressed, this, &AARCharacter::OnLeftMouseButtonPressed);
	PlayerInputComponent->BindAction("MouseLeftButton", IE_Released, this, &AARCharacter::OnLeftMouseButtonReleased);

	PlayerInputComponent->BindAction("Cast", IE_Pressed, this, &AARCharacter::ray);
}


void AARCharacter::ray()
{
	FVector2D MouseScreenPosition;
	if (UGameplayStatics::GetPlayerController(this, 0)->GetMousePosition(MouseScreenPosition.X, MouseScreenPosition.Y))
	{
		FVector WorldOrigin;
		FVector WorldDirection;
		UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(this, 0), MouseScreenPosition, WorldOrigin, WorldDirection);

		FVector Start = WorldOrigin;
		FVector End = Start + (WorldDirection * 1000);

		FHitResult HitResult;

		if (GetWorld())
		{
			bool bActorHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Pawn);
			DrawDebugLine(GetWorld(), Start, End, FColor::White, false, 2.f, 0.0f, 3.0f);


			if (bActorHit && HitResult.GetActor())
			{
				GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::White,
					HitResult.GetActor()->GetName());
			}
		}

		if (GetWorld()->LineTraceSingleByProfile(HitResult, Start, End, FName("Bed")))
		{
			UE_LOG(LogTemp, Warning, TEXT("HitResult"));
		}
	}

}

void AARCharacter::OnLeftMouseButtonPressed()
{
	if (isMoveStart)
	{
		// Ŭ���� ���� �������� (���콺 Ŀ�� �Ʒ��� �ִ�)
		FHitResult HitResult;
		UGameplayStatics::GetPlayerController(this, 0)->GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, HitResult);

		if (HitResult.GetActor() != nullptr && HitResult.GetActor()->IsA<ABed>())
		{
			ClickedActor = HitResult.GetActor();
			DragStartLocation = ClickedActor->GetActorLocation();
			bIsDragging = true;

		}
	}

	if (isRotStart)
	{
		// Ŭ���� ���� �������� (���콺 Ŀ�� �Ʒ��� �ִ�)
		FHitResult HitResult;
		UGameplayStatics::GetPlayerController(this, 0)->GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, HitResult);

		if (HitResult.GetActor() != nullptr && HitResult.GetActor()->IsA<ARotationArrrowActor>())
		{
			ClickedActor = HitResult.GetActor();
			DragStartLocation = ClickedActor->GetActorLocation();
			bIsDragging = true;
			bIsRotOnly = true;


			RotationArrowActor->SetActorHiddenInGame(false);
			// ClickedActor�� �̸��� �α׿� ���
			UE_LOG(LogTemp, Warning, TEXT("Clicked Actor: %s"), *ClickedActor->GetName());

		}
	}


}

void AARCharacter::OnLeftMouseButtonReleased()
{
	// �巡�� ����
	bIsDragging = false;

	if (isMoveStart)
	{
		isMoveStart = false;
		bIsRotOnly = true;
	}


	if (isRotStart)
	{

		RotationArrowActor->SetActorHiddenInGame(true);
		if (bedActor->GetAttachParentActor() == RotationArrowActor)
		{
			bedActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

		}

		FVector arrowLot = bedActor->GetActorLocation();
		arrowLot.Z = 100;

		RotationArrowActor->SetActorLocation(arrowLot);
		bedActor->AttachToActor(RotationArrowActor, FAttachmentTransformRules::KeepWorldTransform);
		bIsRotOnly = false;

	}
}

void AARCharacter::ShowPlaneOutLine()
{
	// ������ ��ü�� �迭�� ���  
	TArray<UARTrackedGeometry*> trackedObjects = UARBlueprintLibrary::GetAllGeometries();

	// ������ ���� �׵θ�(OutLine ǥ��)
	for (UARTrackedGeometry* obj : trackedObjects)
	{
		obj->DebugDraw(GetWorld(), FLinearColor::Yellow, 10, 0);
	}
}



FVector AARCharacter::GetTouchLocation(const FVector2D& touchPos)
{
	if (bCanSpawnActor)
	{
		FVector	touchWorldPos, touchWorldDir;

		// ��ġ�� ��ũ�� ��ǥ�� ���� ��ǥ�� ��ȯ�ϱ�
		bool result = pc->DeprojectScreenPositionToWorld(touchPos.X, touchPos.Y, touchWorldPos, touchWorldDir);

		FHitResult hitInfo;
		FCollisionObjectQueryParams params;
		params.AddObjectTypesToQuery(ECC_WorldDynamic);
		FVector spawnLocation;

		if (GetWorld()->LineTraceSingleByObjectType(hitInfo, touchWorldPos, touchWorldPos + touchWorldDir * 2000, params))
		{
			spawnLocation = hitInfo.GetActor()->GetActorLocation() + hitInfo.ImpactNormal * 20;
			bCanSpawnActor = false;
		}
		return spawnLocation;
	}
	return FVector::ZeroVector;
}


void AARCharacter::MoveForward(float Value)
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

void AARCharacter::MoveRight(float Value)
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

void AARCharacter::Turn(float Value)
{
	CameraComponent->AddRelativeRotation(FRotator(0, Value * 2, 0));

}

void AARCharacter::LookUp(float Value)
{
	FRotator CameraRotation = CameraComponent->GetRelativeRotation();
	CameraRotation.Pitch = FMath::Clamp(CameraRotation.Pitch + Value * 2, -80, 80);
	CameraComponent->SetRelativeRotation(CameraRotation);
}

void AARCharacter::Tilt(float Value)
{
	if (Value != 0.0f)
	{
		FVector TiltMovement = FVector(0, 0, Value * TiltSpeed);
		AddActorLocalOffset(TiltMovement);
	}
}

void AARCharacter::ChangeMovementSpeed(float Value)
{
	MovementSpeed = MovementSpeed + Value * 0.01;
	MovementSpeed = FMath::Clamp(MovementSpeed, 0.01, MaximumMovementSpeed);
}

UWorld* AARCharacter::GetGameWorld()
{
	if (UGameViewportClient* Viewport = GEngine->GameViewport)
	{
		FWorldContext* worldcontext = GEngine->GetWorldContextFromGameViewport(Viewport);
		UWorld* world = worldcontext->World();
		return world;
	}

	return nullptr;
}


bool AARCharacter::CheckHitAtMouseCursor(FHitResult& objecthit)
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


void AARCharacter::DetectMouseHitLocation()
{
	FHitResult HitResult;
	if (CheckHitAtMouseCursor(HitResult))
	{
		if (AActor* HitActor = HitResult.GetActor())
		{
			if (HitActor->IsA(AStaticMeshActor::StaticClass()))
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

void AARCharacter::DetectMouseMoveAtLocation()
{
	FHitResult HitResult;
	if (CheckHitAtMouseCursor(HitResult))
	{
		if (AActor* HitActor = HitResult.GetActor())
		{
			OnHoverActorDetected(HitActor, HitResult.GetComponent());
		}

	}
}

void AARCharacter::UpdateIndicatorRotation()
{
	if (IndicatorActor)
	{
		FVector CameraLocation = CameraComponent->GetComponentLocation();
		FVector IndicatorLocation = IndicatorActor->GetActorLocation();

		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(CameraLocation, IndicatorLocation);
		LookAtRotation.Yaw -= 90;
		IndicatorActor->SetActorRotation(FRotator(0, LookAtRotation.Yaw, 0));
	}
}

void AARCharacter::DestroyIndicator()
{
	if (IndicatorActor)
	{
		IndicatorActor->Destroy();
		IndicatorActor = nullptr;
	}
}

void AARCharacter::PlaceIndicator(const FVector& Location)
{
	// Get a reference to the world
	if (UWorld* World = GetGameWorld())
	{
		if (IndicatorMarkerMesh)
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


void AARCharacter::FBXImport()
{

}


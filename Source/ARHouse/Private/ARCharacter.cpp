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

	// AR 카메라 켜기
	UARBlueprintLibrary::SetEnabledXRCamera(true);

	// 설정된 값으로 AR 세션 실행
	UARBlueprintLibrary::StartARSession(arSessionCinfig1);

	// 핸드폰의 절전 모드를 끄기
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


	// YourActor는 액터의 포인터이며, 이를 사용하여 X 위치를 가져옵니다.
	if (bedActor != nullptr)
	{
		// 액터의 현재 위치를 가져옵니다.
		FVector ActorLocation = bedActor->GetActorLocation();

		// X 위치를 문자열로 변환합니다.
		XLocationString = FString::Printf(TEXT("X: %.2f"), ActorLocation.X);

		//txt_X->SetText(FText::FromString(LocationText));

	}


	// 침대 이동
	if (bIsDragging && ClickedActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("dqwdqwdeqf"));
		if (bIsRotOnly)
		{
			UE_LOG(LogTemp, Warning, TEXT("bIsRotOnly"));
			// 현재 마우스 위치와 이전 마우스 위치 사이의 X 변화량을 계산합니다.
			float DeltaX = DragEndPosition.X - DragStartLocation.X;

			// 회전 속도 조절을 위한 변수 설정
			float RotationSpeed = 1.0f;

			// 프레임당 회전량 계산
			float RotationAmount = -DeltaX * RotationSpeed * GetWorld()->GetDeltaSeconds();  // 음수로 변경

			// 액터의 현재 회전값
			FRotator CurrentRotation = ClickedActor->GetActorRotation();

			// Yaw 회전값을 계산하여 업데이트합니다.
			float NewYaw = CurrentRotation.Yaw + RotationAmount;

			// Yaw 값을 0~360 범위로 제한합니다.
			NewYaw = FMath::Fmod(NewYaw, 360.0f);

			// 음수 값일 경우에는 양수로 변환합니다.
			if (NewYaw < 0)
			{
				NewYaw += 360.0f;
			}

			// 새로운 회전값 생성
			FRotator NewRotation = FRotator(CurrentRotation.Pitch, NewYaw, CurrentRotation.Roll);

			// 액터의 회전 업데이트하기
			//ClickedActor->SetActorRotation(NewRotation);

			RotationArrowActor->SetActorRotation(NewRotation);
		}

		else
		{
			FVector WorldOrigin;
			FVector WorldDirection;

			// 현재 마우스 위치 월드 좌표로 변환하기
			UGameplayStatics::GetPlayerController(this, 0)->DeprojectMousePositionToWorld(WorldOrigin, WorldDirection);

			// 새로운 위치 계산하기
			DragEndPosition = WorldOrigin + (WorldDirection * 1000.f); // 이동 거리 조절 가능

			// 액터의 위치 업데이트하기
			FVector DragDelta = DragEndPosition - DragStartLocation;
			DragDelta.Z = 0;
			ClickedActor->SetActorLocation(DragStartLocation + DragDelta);
			bIsRotOnly = false;
		}

	}


	// 회전 버튼을 누르면 회전 오브젝트가 스폰되게
	if (isRotStart && !RotationArrowActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("sdquwdhk"));
		bedActor = Cast<ABed>(UGameplayStatics::GetActorOfClass(GetWorld(), ABed::StaticClass()));

		if (bedActor)
		{
			FVector SpawnLocation = bedActor->GetActorLocation() + FVector(0, 0, 70);; // 스폰 위치 설정 (bedActor의 위치로 설정)
			FRotator SpawnRotation = FRotator::ZeroRotator; // 스폰 회전 설정

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
		// 클릭된 액터 가져오기 (마우스 커서 아래에 있는)
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
		// 클릭된 액터 가져오기 (마우스 커서 아래에 있는)
		FHitResult HitResult;
		UGameplayStatics::GetPlayerController(this, 0)->GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, HitResult);

		if (HitResult.GetActor() != nullptr && HitResult.GetActor()->IsA<ARotationArrrowActor>())
		{
			ClickedActor = HitResult.GetActor();
			DragStartLocation = ClickedActor->GetActorLocation();
			bIsDragging = true;
			bIsRotOnly = true;


			RotationArrowActor->SetActorHiddenInGame(false);
			// ClickedActor의 이름을 로그에 출력
			UE_LOG(LogTemp, Warning, TEXT("Clicked Actor: %s"), *ClickedActor->GetName());

		}
	}


}

void AARCharacter::OnLeftMouseButtonReleased()
{
	// 드래그 종료
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
	// 감지된 물체를 배열에 담기  
	TArray<UARTrackedGeometry*> trackedObjects = UARBlueprintLibrary::GetAllGeometries();

	// 감지된 면의 테두리(OutLine 표시)
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

		// 터치의 스크린 좌표를 월드 좌표로 변환하기
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


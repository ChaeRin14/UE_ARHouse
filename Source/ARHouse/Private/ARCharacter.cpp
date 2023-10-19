// Fill out your copyright notice in the Description page of Project Settings.


#include "ARCharacter.h"
#include "Components/CapsuleComponent.h"
#include <GameFramework/Pawn.h>
#include <Camera/CameraComponent.h>
#include <GameFramework/SpringArmComponent.h>
#include "ARBlueprintLibrary.h"
#include <Kismet/GameplayStatics.h>
#include "MapToolManager.h"

// Sets default values
AARCharacter::AARCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(RootComponent);
	arCamComp = CreateDefaultSubobject<UCameraComponent>(TEXT("arCamComp"));
	arCamComp->SetupAttachment(SpringArmComp);

	AutoPossessPlayer = EAutoReceiveInput::Player0;
	
	
	
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
	bool bIsFirstTouch;

	pc->GetInputTouchState(ETouchIndex::Touch1, firstTouch.X, firstTouch.Y, bIsFirstTouch);

	if (chair_inst == nullptr && bIsFirstTouch)
	{
		FActorSpawnParameters param;
		param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		GetWorld()->SpawnActor<AActor>(chair_BP, GetTouchLocation(firstTouch), FRotator::ZeroRotator, param);
	}

	if (bIsDragging && ClickedActor)
	{
		FVector WorldOrigin;
		FVector WorldDirection;

		// 현재 마우스 위치 월드 좌표로 변환하기
		UGameplayStatics::GetPlayerController(this, 0)->DeprojectMousePositionToWorld(WorldOrigin, WorldDirection);

		// 새로운 위치 계산하기
		FVector DragEndPosition = WorldOrigin + (WorldDirection * 1000.f); // 이동 거리 조절 가능

		// 액터의 위치 업데이트하기
		FVector DragDelta = DragEndPosition - DragStartLocation;
		DragDelta.Z = 0;
		ClickedActor->SetActorLocation(DragStartLocation + DragDelta);
	}

}

// Called to bind functionality to input
void AARCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);


	PlayerInputComponent->BindAxis(TEXT("Horizontal"), this, &AARCharacter::Horizontal);
	PlayerInputComponent->BindAxis(TEXT("Vertical"), this, &AARCharacter::Vertical);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &AARCharacter::Turn);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &AARCharacter::LookUp);
	PlayerInputComponent->BindAxis(TEXT("Zaxis"), this, &AARCharacter::Zaxis);

	PlayerInputComponent->BindAction("MouseLeftButton", IE_Pressed, this, &AARCharacter::OnLeftMouseButtonPressed);
	PlayerInputComponent->BindAction("MouseLeftButton", IE_Released, this, &AARCharacter::OnLeftMouseButtonReleased);

	PlayerInputComponent->BindAction("Cast", IE_Pressed, this, &AARCharacter::ray);
}


void AARCharacter::Horizontal(float value)
{
	AddMovementInput(GetActorRightVector(), value);
}

void AARCharacter::Vertical(float value)
{
	AddMovementInput(GetActorForwardVector(), value);
}

void AARCharacter::Turn(float value)
{
	AddControllerYawInput(value);
}

void AARCharacter::LookUp(float AxisValue)
{
	FRotator NewRotation = GetControlRotation();
	NewRotation.Pitch = FMath::Clamp(NewRotation.Pitch + AxisValue, -90.0f, 90.0f);
	SetActorRotation(NewRotation);
}

void AARCharacter::Zaxis(float value)
{
	AddMovementInput(GetActorUpVector(), value);
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
				GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::White,
					HitResult.GetActor()->GetName());
			}
		}

		if (GetWorld()->LineTraceSingleByProfile(HitResult, Start, End, FName("Bed")))
		{
			UE_LOG(LogTemp, Warning, TEXT("HitResult"));
		}
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
	if(bCanSpawnActor)
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


void AARCharacter::OnLeftMouseButtonPressed()
{
	// 클릭된 액터 가져오기 (마우스 커서 아래에 있는)
	FHitResult HitResult;
	UGameplayStatics::GetPlayerController(this, 0)->GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, HitResult);

	if (HitResult.GetActor() && HitResult.GetActor()->IsA<ABed>())
	{
		ClickedActor = HitResult.GetActor();
		DragStartLocation = ClickedActor->GetActorLocation();
		bIsDragging = true;
	}
}

void AARCharacter::OnLeftMouseButtonReleased()
{
	// 드래그 종료
	bIsDragging = false;
	UE_LOG(LogTemp, Warning, TEXT("sdqasdqwd"));
}

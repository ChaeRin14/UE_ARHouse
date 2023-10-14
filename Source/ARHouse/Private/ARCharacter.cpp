// Fill out your copyright notice in the Description page of Project Settings.


#include "ARCharacter.h"
#include "Components/CapsuleComponent.h"
#include <GameFramework/Pawn.h>
#include <Camera/CameraComponent.h>
#include <GameFramework/SpringArmComponent.h>
#include "ARBlueprintLibrary.h"
#include <Kismet/GameplayStatics.h>
#include "MapToolManager.h"
#include "Widget_CreateRoom.h"

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

	//ShowPlaneOutLine();
	SetIndicator();

	FVector2D firstTouch;
	bool bIsFirstTouch;

	pc->GetInputTouchState(ETouchIndex::Touch1, firstTouch.X, firstTouch.Y, bIsFirstTouch);

	if (chair_inst == nullptr && bIsFirstTouch)
	{
		FActorSpawnParameters param;
		param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		GetWorld()->SpawnActor<AActor>(chair_BP, GetTouchLocation(firstTouch), FRotator::ZeroRotator, param);
	}

// 	mapToolManager = Cast<AMapToolManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AMapToolManager::StaticClass()));
// 
// 	if (mapToolManager->isAClick || mapToolManager->isDClick)
// 	{
// 		FVector Ap0 = GetActorForwardVector();
// 		FVector Avt = Dir * 500 * DeltaTime;
// 		FVector Ap = Ap0 + Avt;
// 		SetActorLocation(Ap, true);
// 	}
// 
// 	if (mapToolManager->isWClick)
// 	{
// 		AddMovementInput(GetActorForwardVector(), speed);
// 	}
// 
// 	if (mapToolManager->isSClick)
// 	{
// 		AddMovementInput(-GetActorForwardVector(), speed);
// 	}
// 
// 	if (mapToolManager->isAClick)
// 	{
// 		AddMovementInput(-GetActorForwardVector(), speed);
// 	}
// 
// 	if (mapToolManager->isDClick)
// 	{
// 		AddMovementInput(GetActorRightVector(), speed);
// 	}

}

// Called to bind functionality to input
void AARCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);


	PlayerInputComponent->BindAxis(TEXT("Horizontal"), this, &AARCharacter::Horizontal);
	PlayerInputComponent->BindAxis(TEXT("Vertical"), this, &AARCharacter::Vertical);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &AARCharacter::Turn);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &AARCharacter::LookUp);

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

void AARCharacter::LookUp(float value)
{
	AddControllerPitchInput(value);
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

void AARCharacter::SetIndicator()
{
	if(indicator == nullptr) return;
	// 화면에 추적된 면이 있는지 AR 라인 트레이스로 검색한다.
	TArray<FARTraceResult> hitInfos = UARBlueprintLibrary::LineTraceTrackedObjects(FVector2D(2560 / 2, 1440 / 2), false, true, false, false);

	if (hitInfos.Num() > 0)
	{
		// 인디케이터가 없다면 생성한다.
		if (spawnedIndicator == nullptr)
		{ 
			if (widget_BP)
			{
				widget_inst = (CreateWidget<UUserWidget>(GetWorld(), widget_BP));
				if (widget_inst)
				{

					// 위젯을 화면에 추가
					widget_inst->AddToViewport();
				}
			}
			spawnedIndicator = GetWorld()->SpawnActor<AActor>(indicator, hitInfos[0].GetLocalToWorldTransform());
		}
		// 인디케이터가 있다면 Location 값만 변경한다. 
		else
		{
			widget_inst->SetVisibility(ESlateVisibility::Visible);
			spawnedIndicator->SetActorHiddenInGame(false);
			spawnedIndicator->SetActorTransform(hitInfos[0].GetLocalToWorldTransform());
		}
	}
	else
	{
		if (spawnedIndicator != nullptr)
		{
			//위젯, 인디케이터 Hidden
			widget_inst->SetVisibility(ESlateVisibility::Hidden);
			spawnedIndicator->SetActorHiddenInGame(true);
		}
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

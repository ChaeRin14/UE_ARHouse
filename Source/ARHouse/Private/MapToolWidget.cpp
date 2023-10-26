// Fill out your copyright notice in the Description page of Project Settings.


#include "MapToolWidget.h"
#include "Components/Button.h"
#include "ARCharacter.h"
#include <Kismet/GameplayStatics.h>
#include "Bed.h"
#include "MapToolManager.h"
#include "RotationArrrowActor.h"

void UMapToolWidget::NativeConstruct()
{
	Super::NativeConstruct();


	// 가구
	btn_furniture->OnClicked.AddDynamic(this, &UMapToolWidget::Object_furniture);
	// 조명
	btn_light->OnClicked.AddDynamic(this, &UMapToolWidget::Object_light);
	// 벽지
	btn_wallpaper->OnClicked.AddDynamic(this, &UMapToolWidget::Object_wallpaper);
	// 템플릿
	btn_template->OnClicked.AddDynamic(this, &UMapToolWidget::Object_template);

	// 가구 - 침대
	btn_bed->OnClicked.AddDynamic(this, &UMapToolWidget::Spawn_Bed);

	// 오브젝트 이동
	btn_Move->OnClicked.AddDynamic(this, &UMapToolWidget::MoveObj);
	// 오브젝트 회전
	btn_Rotation->OnClicked.AddDynamic(this, &UMapToolWidget::RotObj);


	btn_save->OnClicked.AddDynamic(this, &UMapToolWidget::Object_save);

	// 침대 버튼 꺼 두기
	btn_bed->SetVisibility(ESlateVisibility::Hidden);


	player = Cast<AARCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
}

void UMapToolWidget::Object_furniture()
{
	btn_bed->SetVisibility(ESlateVisibility::Visible);
}

void UMapToolWidget::Object_light()
{

}

void UMapToolWidget::Object_wallpaper()
{

}

void UMapToolWidget::Object_template()
{

}



void UMapToolWidget::Spawn_Bed()
{
	// 플레이어 컨트롤러 가져오기
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (!PlayerController)
	{
		return;
	}

	// 플레이어 컨트롤러의 위치와 앞 방향 가져오기
	FVector PlayerLocation;
	FRotator PlayerRotation;
	PlayerController->GetPlayerViewPoint(PlayerLocation, PlayerRotation);

	// 액터 스폰 위치 계산 (예: 플레이어 위치에서 앞으로 100 단위만큼 이동)
	FVector SpawnLocation = PlayerLocation + PlayerRotation.Vector() * 300.f;
	SpawnLocation.Z = 50.0f;

	// 액터 스폰
	bedActor = GetWorld()->SpawnActor<ABed>(ABed::StaticClass(), SpawnLocation, FRotator::ZeroRotator);


}

void UMapToolWidget::MoveObj()
{
	if (player)
	{
		player->isMoveStart = true;
	}

}

void UMapToolWidget::RotObj()
{
	if (player != nullptr)
	{
		player->isRotStart = true;
		player->bIsRotOnly = true;

		if (player->RotationArrowActor != nullptr && player->RotationArrowActor->IsHidden())
		{

			player->RotationArrowActor->SetActorHiddenInGame(false);
		}
	}
}

void UMapToolWidget::Object_save()
{
	if (player != nullptr)
	{
		savelot = player->GetActorLocation();
		UE_LOG(LogTemp, Warning, TEXT("Save player Location: %s"), *savelot.ToString());

		UGameplayStatics::OpenLevel(GetWorld(), TEXT("ARHouse"));
	}

}


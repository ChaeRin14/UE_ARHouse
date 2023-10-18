// Fill out your copyright notice in the Description page of Project Settings.


#include "MapToolWidget.h"
#include "Components/Button.h"
#include "ARCharacter.h"
#include <Kismet/GameplayStatics.h>
#include "Bed.h"
#include "MapToolManager.h"

void UMapToolWidget::NativeConstruct()
{
	Super::NativeConstruct();


	// ����
	btn_furniture->OnClicked.AddDynamic(this, &UMapToolWidget::Object_furniture);
	// ����
	btn_light->OnClicked.AddDynamic(this, &UMapToolWidget::Object_light);
	// ����
	btn_wallpaper->OnClicked.AddDynamic(this, &UMapToolWidget::Object_wallpaper);
	// ���ø�
	btn_template->OnClicked.AddDynamic(this, &UMapToolWidget::Object_template);

	// ���� - ħ��
	btn_bed->OnClicked.AddDynamic(this, &UMapToolWidget::Spawn_Bed);
	// ħ�� ��ư �� �α�
	btn_bed->SetVisibility(ESlateVisibility::Hidden);
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
	// �÷��̾� ��Ʈ�ѷ� ��������
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (!PlayerController)
	{
		return;
	}

	// �÷��̾� ��Ʈ�ѷ��� ��ġ�� �� ���� ��������
	FVector PlayerLocation;
	FRotator PlayerRotation;
	PlayerController->GetPlayerViewPoint(PlayerLocation, PlayerRotation);

	// ���� ���� ��ġ ��� (��: �÷��̾� ��ġ���� ������ 100 ������ŭ �̵�)
	FVector SpawnLocation = PlayerLocation + PlayerRotation.Vector() * 300.f;
	SpawnLocation.Z = 50.0f;

	// ���� ����
	bedActor = GetWorld()->SpawnActor<ABed>(ABed::StaticClass(), SpawnLocation, FRotator::ZeroRotator);

	
}


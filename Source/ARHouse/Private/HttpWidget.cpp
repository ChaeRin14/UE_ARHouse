// Fill out your copyright notice in the Description page of Project Settings.


#include "HttpWidget.h"
#include "Components/Button.h"
#include "Widget_CreateRoom.h"
#include "ARCharacter.h"
#include <Kismet/GameplayStatics.h>

void UHttpWidget::NativeConstruct()
{
	Super::NativeConstruct();

	btn_w->OnClicked.AddDynamic(this, &UHttpWidget::w_Move);
	btn_s->OnClicked.AddDynamic(this, &UHttpWidget::s_Move);
	btn_a->OnClicked.AddDynamic(this, &UHttpWidget::a_Move);
	btn_d->OnClicked.AddDynamic(this, &UHttpWidget::d_Move);


	player = Cast<AARCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
}

void UHttpWidget::loadBedSpawn()
{
	/*if (!createWidget->IsInViewport())
	{
	UE_LOG(LogTemp, Warning, TEXT("adwdqwqw")); 
		createWidget->isObjSpawn = true;
	}*/
}

void UHttpWidget::w_Move()
{
UE_LOG(LogTemp, Warning, TEXT("dqowhdqowd"));
	player->MoveForward(5.0f);
}

void UHttpWidget::a_Move()
{
	player->MoveRight(-5.0f);
}

void UHttpWidget::s_Move()
{
	player->MoveForward(-5.0f);
}

void UHttpWidget::d_Move()
{
	player->MoveRight(5.0f);
}

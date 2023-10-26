// Fill out your copyright notice in the Description page of Project Settings.


#include "HttpWidget.h"
#include "Components/Button.h"

#include "Widget_CreateRoom.h"

void UHttpWidget::NativeConstruct()
{
	Super::NativeConstruct();

	/*btn_loadSpawn->OnClicked.AddDynamic(this, &UHttpWidget::loadBedSpawn);
	createWidget = CreateWidget<UWidget_CreateRoom>(GetWorld(), UWidget_CreateRoom::StaticClass());*/
}

void UHttpWidget::loadBedSpawn()
{
	/*if (!createWidget->IsInViewport())
	{
	UE_LOG(LogTemp, Warning, TEXT("adwdqwqw")); 
		createWidget->isObjSpawn = true;
	}*/
}

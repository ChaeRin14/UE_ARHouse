// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget_CreateRoom.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "ARCharacter.h"
#include <Kismet/GameplayStatics.h>

void UWidget_CreateRoom::NativeConstruct()
{
	btn_MakeRoom->OnClicked.AddDynamic(this, &UWidget_CreateRoom::OnClickCreateButton);

}

void UWidget_CreateRoom::OnClickCreateButton()
{
	UE_LOG(LogTemp, Warning,TEXT("Click!!!"));
	UGameplayStatics::OpenLevel(GetWorld(), FName("ARHouse"));
}

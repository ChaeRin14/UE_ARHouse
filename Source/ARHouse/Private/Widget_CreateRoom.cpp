// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget_CreateRoom.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "ARCharacter.h"
#include <Kismet/GameplayStatics.h>
#include "Components/Image.h"
#include "Components/Throbber.h"

void UWidget_CreateRoom::NativeConstruct()
{
	btn_MakeRoom->OnClicked.AddDynamic(this, &UWidget_CreateRoom::OnClickCreateButton);
	
	img_Loading->SetVisibility(ESlateVisibility::Hidden);
	t_Loading->SetVisibility(ESlateVisibility::Hidden);
}

void UWidget_CreateRoom::OnClickCreateButton()
{
	

	PlayAnimation(a_fade);
 	img_Loading->SetVisibility(ESlateVisibility::Visible);
 	t_Loading->SetVisibility(ESlateVisibility::Visible);

	FTimerHandle DelayHandle;
	GetWorld()->GetTimerManager().SetTimer(DelayHandle, FTimerDelegate::CreateLambda([&]()
		{
			UGameplayStatics::OpenLevel(GetWorld(), FName("ARHouse"));
		}
	), 10, false);
	
}

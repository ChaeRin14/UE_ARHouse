// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget_CreateRoom.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "ARCharacter.h"

void UWidget_CreateRoom::NativeConstruct()
{
	if(btn_MakeRoom != nullptr)
	{ 
		btn_MakeRoom->OnClicked.AddDynamic(this, &UWidget_CreateRoom::OnClickCreateButton);
	}
}

void UWidget_CreateRoom::OnClickCreateButton()
{
	/*bIsClicked = true;*/
}

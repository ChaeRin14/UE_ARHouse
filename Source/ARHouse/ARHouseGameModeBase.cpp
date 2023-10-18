// Copyright Epic Games, Inc. All Rights Reserved.


#include "ARHouseGameModeBase.h"
#include "MainWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"


void AARHouseGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	httpUI = CreateWidget<UMainWidget>(GetWorld(), httpWidget);

	if (httpUI != nullptr)
	{
		httpUI->AddToViewport();
	}

	GetWorld()->GetFirstPlayerController()->SetShowMouseCursor(true);
	GetWorld()->GetFirstPlayerController()->SetInputMode(FInputModeUIOnly());
}

// �α� ��� �Լ�
void AARHouseGameModeBase::SetLogText(const FString msg)
{
	//httpUI->text_log->SetText(FText::FromString(msg));
}

// �ؽ�ó�� �̹��� ������Ʈ�� �����ϴ� �Լ�
void AARHouseGameModeBase::SetImageTexture(class UTexture2D* tex)
{
	if (httpUI != nullptr)
	{
		//httpUI->img_Http->SetBrushFromTexture(tex);
	}
}

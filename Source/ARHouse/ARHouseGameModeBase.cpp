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

// 로그 출력 함수
void AARHouseGameModeBase::SetLogText(const FString msg)
{
	//httpUI->text_log->SetText(FText::FromString(msg));
}

// 텍스처를 이미지 컴포넌트에 설정하는 함수
void AARHouseGameModeBase::SetImageTexture(class UTexture2D* tex)
{
	if (httpUI != nullptr)
	{
		//httpUI->img_Http->SetBrushFromTexture(tex);
	}
}

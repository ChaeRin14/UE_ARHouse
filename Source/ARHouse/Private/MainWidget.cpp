// Fill out your copyright notice in the Description page of Project Settings.


#include "MainWidget.h"
#include <UMG/Public/Components/Button.h>

void UMainWidget::NativeConstruct()
{

	Super::NativeConstruct();

	// 이미지 업로드
	btn_fileImage->OnClicked.AddDynamic(this, &UMainWidget::FileRequest);
}

void UMainWidget::FileRequest()
{
	FString DefaultPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir());
	FString Directory;

	bool IsSelect = true;
	httpReqActor->FileOn(DefaultPath, Directory, IsSelect);


	if (httpReqActor != nullptr)
	{
		FString fullurl = httpReqActor->DestinationFilePath;

		httpReqActor->GetFileImage(httpReqActor->DestinationFilePath);
		UE_LOG(LogTemp, Warning, TEXT("FileRequest!!!!! : %s"), *fullurl);
	}
}

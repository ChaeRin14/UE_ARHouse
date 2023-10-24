// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainWidget.generated.h"

/**
 *
 */
UCLASS()
class ARHOUSE_API UMainWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = MySettings)
	class UButton* btn_fileImage;


	FString AbsolutePath;
	FString DestinationFilePath;
	FString SourceFilePath;

	FVector save;

private:
	UFUNCTION()
	void FileOn();
	void PostImageRequest();

	
	UPROPERTY()
	class AHttpRequestActor* httpReqActor;

	// ���� api
	FString baseURL = "192.168.0.50:8080/ai/drawing/process";
};

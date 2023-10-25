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


private:
	UFUNCTION()
	void FileOn();
	void PostImageRequest();

	
	UPROPERTY()
	class AHttpRequestActor* httpReqActor;

	// ¼­¹ö api
	FString baseURL = "172.17.107.149:8080/ai/drawing/process";
};

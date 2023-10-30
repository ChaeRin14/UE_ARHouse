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

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = MySettings)
	class UButton* MapLoadBtn;
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = MySettings)
	class UImage* LoadImage;
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = MySettings)
	class UTextBlock* txt_MapLoad;

	FString AbsolutePath;
	FString DestinationFilePath;
	FString SourceFilePath;

	// ¼­¹ö api
	FString baseURL = "192.168.0.47:8080/ai/drawing/process";
private:
	UFUNCTION()
	void FileOn();
	UFUNCTION()
	void PostImageRequest();
	UFUNCTION()
	void LoadMap();

	
	UPROPERTY()
	class AHttpRequestActor* httpReqActor;

	
};

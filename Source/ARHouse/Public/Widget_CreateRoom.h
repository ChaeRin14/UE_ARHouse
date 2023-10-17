// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_CreateRoom.generated.h"

/**
 * 
 */
UCLASS()
class ARHOUSE_API UWidget_CreateRoom : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

public:
	UPROPERTY(VisibleAnywhere, meta=(BindWidget), Category="MySettings")
	class UButton* btn_MakeRoom;

	UPROPERTY(VisibleAnywhere, meta=(BindWidget), Category="MySettings")
	class UTextBlock* text_MakeRoom;

	UPROPERTY(VisibleAnywhere, meta=(BindWidget), Category="MySettings")
	class UImage* img_Loading;

	UPROPERTY(VisibleAnywhere, meta=(BindWidget), Category="MySettings")
	class UThrobber* t_Loading;

	UPROPERTY(VisibleAnywhere, Transient, meta = (BindWidgetAnim), Category = "MySettings")
	class UWidgetAnimation* a_fade;

	UPROPERTY(EditAnywhere, Category="MySettings")
	bool bIsClicked = false;

	UFUNCTION()
	void OnClickCreateButton();

};

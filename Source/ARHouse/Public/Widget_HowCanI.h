// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_HowCanI.generated.h"

/**
 * 
 */
UCLASS()
class ARHOUSE_API UWidget_HowCanI : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	
	virtual void NativeConstruct() override;

public:
	
	UPROPERTY(VisibleAnywhere, meta=(BindWidget), Category="MySettings")
	class UImage* img_Light;

	UPROPERTY(VisibleAnywhere, meta=(BindWidget), Category="MySettings")
	class UImage* img_Phone;

	UPROPERTY(VisibleAnywhere, meta=(BindWidget), Category="MySettings")
	class UImage* img_Floor;

	UPROPERTY(VisibleAnywhere, Transient, meta = (BindWidgetAnim), Category = "MySettings")
	class UWidgetAnimation* a_HowCanI;

	UPROPERTY(VisibleAnywhere, meta=(BindWidget), Category="MySettings")
	class UTextBlock* text_guide;
};

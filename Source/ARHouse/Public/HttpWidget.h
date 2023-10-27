// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ARCharacter.h"
#include "HttpWidget.generated.h"

/**
 * 
 */
UCLASS()
class ARHOUSE_API UHttpWidget : public UUserWidget
{
	GENERATED_BODY()


public:
	virtual void NativeConstruct() override;
	
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = MySettings)
	class UButton* btn_loadSpawn;

	
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = MySettings)
	class UButton* btn_w;
	
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = MySettings)
	class UButton* btn_a;
	
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = MySettings)
	class UButton* btn_s;
	
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = MySettings)
	class UButton* btn_d;

	void loadBedSpawn();

	UFUNCTION()
	void w_Move();
	UFUNCTION()
	void a_Move();
	UFUNCTION()
	void s_Move();
	UFUNCTION()
	void d_Move();

	class UWidget_CreateRoom* createWidget;

	UPROPERTY(BlueprintReadWrite)
	FVector newLot;

	 AARCharacter* player;
};

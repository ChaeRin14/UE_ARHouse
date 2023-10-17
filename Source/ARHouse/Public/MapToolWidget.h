// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MapToolWidget.generated.h"

/**
 * 
 */
UCLASS()
class ARHOUSE_API UMapToolWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	// ����
	UPROPERTY(VisibleAnywhere, meta=(BindWidget), Category = MySettings)
	class UButton* btn_furniture;
	// ����
	UPROPERTY(VisibleAnywhere, meta=(BindWidget), Category = MySettings)
	class UButton* btn_light;
	// ����
	UPROPERTY(VisibleAnywhere, meta=(BindWidget), Category = MySettings)
	class UButton* btn_wallpaper;
	// ���ø�
	UPROPERTY(VisibleAnywhere, meta=(BindWidget), Category = MySettings)
	class UButton* btn_template;

	// ���� - ħ��
	UPROPERTY(VisibleAnywhere, meta=(BindWidget), Category = MySettings)
	class UButton* btn_bed;

	class ABed* bedActor;
	bool isbedSpawn;

private:
	UFUNCTION()
	void Object_furniture();
	UFUNCTION()
	void Object_light();
	UFUNCTION()
	void Object_wallpaper();
	UFUNCTION()
	void Object_template();

	UFUNCTION()
	void Spawn_Bed();


	class AARCharacter* player;
	
	
};

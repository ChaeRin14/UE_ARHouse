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

	// 가구
	UPROPERTY(VisibleAnywhere, meta=(BindWidget), Category = MySettings)
	class UButton* btn_furniture;
	// 조명
	UPROPERTY(VisibleAnywhere, meta=(BindWidget), Category = MySettings)
	class UButton* btn_light;
	// 벽지
	UPROPERTY(VisibleAnywhere, meta=(BindWidget), Category = MySettings)
	class UButton* btn_wallpaper;
	// 템플릿
	UPROPERTY(VisibleAnywhere, meta=(BindWidget), Category = MySettings)
	class UButton* btn_template;

	// 가구 - 침대
	UPROPERTY(VisibleAnywhere, meta=(BindWidget), Category = MySettings)
	class UButton* btn_bed;

	// 오브젝트 이동 버튼
	UPROPERTY(VisibleAnywhere, meta=(BindWidget), Category = MySettings)
	class UButton* btn_Move;
	// 오브젝트 회전 버튼
	UPROPERTY(VisibleAnywhere, meta=(BindWidget), Category = MySettings)
	class UButton* btn_Rotation;

	
	UPROPERTY(VisibleAnywhere, meta=(BindWidget), Category = MySettings)
	class UButton* btn_save;

	class ABed* bedActor;
	bool isbedSpawn;

	FVector savelot;
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

	UFUNCTION()
	void MoveObj();
	UFUNCTION()
	void RotObj();

	
	UFUNCTION()
	void Object_save();


	class AARCharacter* player;
	
	
};

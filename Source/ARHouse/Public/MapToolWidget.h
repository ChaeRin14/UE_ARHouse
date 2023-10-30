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
	
	// ī�װ� : ħ��
	UPROPERTY(VisibleAnywhere, meta=(BindWidget), Category = MySettings)
	class UButton* Cbtn_Bed;
	// ����
	UPROPERTY(VisibleAnywhere, meta=(BindWidget), Category = MySettings)
	class UButton* Cbtn_Sofa;
	// ���̺�
	UPROPERTY(VisibleAnywhere, meta=(BindWidget), Category = MySettings)
	class UButton* Cbtn_Table;

	// ���� - ħ��
	UPROPERTY(VisibleAnywhere, meta=(BindWidget), Category = MySettings)
	class UButton* btn_bed;



	

	// ������Ʈ �̵� ��ư
	UPROPERTY(VisibleAnywhere, meta=(BindWidget), Category = MySettings)
	class UButton* btn_Move;
	// ������Ʈ ȸ�� ��ư
	UPROPERTY(VisibleAnywhere, meta=(BindWidget), Category = MySettings)
	class UButton* btn_Rotation;

	
	// ��ǥ �� ����
	UPROPERTY(VisibleAnywhere, meta=(BindWidget), Category = MySettings)
	class UTextBlock* txt_X;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget), Category = MySettings)
	class UTextBlock* txt_Y;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget), Category = MySettings)
	class UTextBlock* txt_Z;

	class ABed* bedActor;
	bool isbedSpawn;
	
	UPROPERTY(BlueprintReadWrite)
	bool isWidgetChange = false;

	UPROPERTY(BlueprintReadWrite)
	FVector savelot;

	UFUNCTION()
	void XLot(FText NewText);
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

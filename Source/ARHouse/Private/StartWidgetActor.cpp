


#include "StartWidgetActor.h"
#include "Widget_CreateRoom.h"
#include "ARBlueprintLibrary.h"
#include "Widget_HowCanI.h"
#include "Components/TextBlock.h"
#include <Sound/SoundCue.h>
#include <Kismet/GameplayStatics.h>


AStartWidgetActor::AStartWidgetActor()
{
 	
	PrimaryActorTick.bCanEverTick = true;

}


void AStartWidgetActor::BeginPlay()
{
	Super::BeginPlay();
	
	if (widget_How)
	{
		widget_Howinst = (CreateWidget<UUserWidget>(GetWorld(), widget_How));
		if(widget_Howinst)
		{
			widget_Howinst->AddToViewport();
		}
	}

	// ���� ���
	if (soundBGM)
	{
		UGameplayStatics::PlaySound2D(this, soundBGM, 1.0f, 1.0f, 0.0f);
	}
}


void AStartWidgetActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//ShowPlaneOutLine();
	if(bisRemove == true)
	{
		SetIndicator();
	}

	
	
	
}

void AStartWidgetActor::SetIndicator()
{
		if (previewActor == nullptr) return;
		if (indicator == nullptr) return;
		// ȭ�鿡 ������ ���� �ִ��� AR ���� Ʈ���̽��� �˻��Ѵ�.
		TArray<FARTraceResult> hitInfos = UARBlueprintLibrary::LineTraceTrackedObjects(FVector2D(2560 / 2, 1440 / 2), false, true, false, false);
		UWidget_HowCanI* Widget_HowCanI = NewObject<UWidget_HowCanI>();

		if (hitInfos.Num() > 0)
		{
			// �ε������Ͱ� ���ٸ� �����Ѵ�.
			if (spawnedIndicator == nullptr && spawnedpreviewActor == nullptr)
			{
				if (widget_BP)
				{
					widget_inst = (CreateWidget<UUserWidget>(GetWorld(), widget_BP));
					if (widget_inst && Widget_HowCanI)
					{
						// ������ ȭ�鿡 �߰�
						widget_Howinst->SetVisibility(ESlateVisibility::Hidden);
						widget_inst->AddToViewport();
					}
				}
				spawnedIndicator = GetWorld()->SpawnActor<AActor>(indicator, hitInfos[0].GetLocalToWorldTransform());
				spawnedIndicator->SetActorHiddenInGame(true);
				
				spawnedpreviewActor = GetWorld()->SpawnActor<AActor>(previewActor, hitInfos[0].GetLocalToWorldTransform());
			}
			// �ε������Ͱ� �ִٸ� Location ���� �����Ѵ�. 
			else
			{
				widget_inst->SetVisibility(ESlateVisibility::Visible);
				widget_Howinst->SetVisibility(ESlateVisibility::Hidden);
				spawnedIndicator->SetActorHiddenInGame(true);
				spawnedIndicator->SetActorTransform(hitInfos[0].GetLocalToWorldTransform());
			}
		}
		else
		{
			if (spawnedIndicator != nullptr)
			{
				//����, �ε������� Hidden
				widget_inst->SetVisibility(ESlateVisibility::Hidden);
				spawnedIndicator->SetActorHiddenInGame(true);
				widget_Howinst->SetVisibility(ESlateVisibility::Visible);
			}
		}

}

void AStartWidgetActor::RemoveUI()
{
	if (widget_Howinst != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Remove!!"));
// 		bisRemove = false;
// 		widget_Howinst->SetVisibility(ESlateVisibility::Hidden);
	}
	
}

void AStartWidgetActor::Preview()
{
	/*if (previewActor == nullptr) return;*/
// 	// ȭ�鿡 ������ ���� �ִ��� AR ���� Ʈ���̽��� �˻��Ѵ�.
// 	TArray<FARTraceResult> hitInfos = UARBlueprintLibrary::LineTraceTrackedObjects(FVector2D(2560 / 2, 1440 / 2), false, true, false, false);
// 	if (hitInfos.Num() > 0)
// 	{
		// �ε������Ͱ� ���ٸ� �����Ѵ�.
/*		if (spawnedpreviewActor == nullptr)*/
/*		{*/
// 			UE_LOG(LogTemp, Warning, TEXT("Spawn Preview"));
// 			spawnedpreviewActor = GetWorld()->SpawnActor<AActor>(previewActor, GetActorLocation() , GetActorRotation());
// 			UE_LOG(LogTemp, Warning, TEXT("Spawn Complete!"));
/*		}*/
/*	}*/
}


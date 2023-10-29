


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
	SetIndicator();
}

void AStartWidgetActor::SetIndicator()
{
		if (indicator == nullptr) return;
		// ȭ�鿡 ������ ���� �ִ��� AR ���� Ʈ���̽��� �˻��Ѵ�.
		TArray<FARTraceResult> hitInfos = UARBlueprintLibrary::LineTraceTrackedObjects(FVector2D(2560 / 2, 1440 / 2), false, true, false, false);
		UWidget_HowCanI* Widget_HowCanI = NewObject<UWidget_HowCanI>();

		if (hitInfos.Num() > 0)
		{
			// �ε������Ͱ� ���ٸ� �����Ѵ�.
			if (spawnedIndicator == nullptr)
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


